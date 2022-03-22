import threading
import socket
import time
import os
import subprocess  # For executing a shell command

from util import is_windows
from constants import (BETWEEN_MESSAGE_SETS_SEP, BETWEEN_MESSAGES_SEP, MESSAGE_SET_START)

COMMS_TIMEOUT = 4
PING_SILENT = True

class PullDataTimedOutException(Exception):
    pass


class TelemetryClient(threading.Thread):
    def __init__(self, data, server_ip, server_port):
        super().__init__()
        self.data = data
        self.connected = False
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_ip = server_ip
        self.server_port = server_port
        self.killme = False
        self.disconnectme = False
        self.last_pinged_robot = None
        self.already_cleared_data = False

    def kill_thread(self):
        self.killme = True
    
    def disconnect_when_convenient(self):
        self.disconnectme = True

    def ping_robot(self):
        """
        Returns True if host responds to a ping request.
        Remember that a host may not respond to a ping (ICMP) request even if the host name is valid.
        """

        try:
            # Option for the number of packets as a function of
            param = '-n' if is_windows() else '-c'

            # Building the command. Ex: "ping -c 1 google.com"
            command = ['ping', param, '1', self.server_ip]

            if PING_SILENT:
                return subprocess.call(command, stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL) == 0
            else:
                return subprocess.call(command) == 0
        except Exception as e:
            print(f'failed to ping robot, {e}')
            return False

    def run(self):
        longest_pull = 0
        longest_push = 0
        while True:
            if self.killme:
                print('killing')
                break
            if self.disconnectme:
                self.disconnect()
                self.disconnectme = False
            if self.connected:
                encoded_out = self.push()
                encoded_in = self.pull()
                
                if self.data.recording_to_dirname is not None:
                    print('writing data')
                    if encoded_in is None:
                        encoded_in = b'None'
                    elif encoded_out is None:
                        encoded_out = b'None'
                    self.data.append_pb_data_to_file(data_received=encoded_in, data_sent=encoded_out)

            else:
                self.append_pb_vals()
                time.sleep(0.1)

    def connect(self):
        try:
            print('connecting')
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(COMMS_TIMEOUT)
            print(self.server_ip)
            print(self.server_port)

            self.socket.connect((self.server_ip,self.server_port))
            self.socket.settimeout(COMMS_TIMEOUT)
            self.connected = True

        except Exception as e:
            print(f'error connecting: {e}')
            self.connected = False
            try:
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.socket.settimeout(COMMS_TIMEOUT)
                self.socket.connect((self.server_ip,self.server_port))
                self.socket.settimeout(COMMS_TIMEOUT)
                self.connected = True
            except Exception as e:
                print(f'error connecting AGAIN! {e}')
                self.connected = False

        print(f'connected: {self.connected}')

    def disconnect(self):
        try:
            self.socket.shutdown(socket.SHUT_RDWR)
            self.socket.close()
        except Exception as e:
            print(f"Failed to disconnect comms: {e}")
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            time.sleep(2)
        print('disconnected')
        self.connected = False

    def push(self):
        try:
            #  print('push')
            encoded = self.data.encode_outgoing()
            self.socket.sendall(encoded)
            return encoded
        except Exception as e:
            print(f"Failed to push data: {e}")
            print(f"Attempting to reconnect.")
            self.connect()
            return None
            #  self.controls.connect_button.refresh_state()

    def pull_raw(self):
        if self.data.cmd.pb.disableTelemetry and self.already_cleared_data:
            return None
        else:
            self.already_cleared_data = False
        try:
            rx_raw = self.socket.recv(1000)
            self.last_pinged_robot = time.time()
            return rx_raw
        except Exception as e:
            if type(e) is socket.timeout:
                print('Pull data timed out!!!')
                if not self.data.cmd.pb.disableTelemetry:
                    self.connected = False
                    raise PullDataTimedOutException
                else:
                    print('Telemetry is disabled, so thats probably fine.')
                    self.already_cleared_data = True
                    t = time.time()
                    if self.last_pinged_robot is not None:
                        dt = t - self.last_pinged_robot
                        should_ping = dt > COMMS_TIMEOUT
                    else:
                        dt = '[literally never]'
                        should_ping = True
                    if should_ping:
                        print(f'Last robot heartbeat was {dt} seconds ago.')
                        print('Let\'s ping the robot to make sure its still vibing:')
                    if self.ping_robot():
                        print("Ping succesful.")
                        self.last_pinged_robot = t
                    else:
                        print("Ping unsuccesful... ok guess we are disconnected.")
                        self.connected = False
            else:
                print(f'ERRORRRRRRRRRR: {e}')
        return None

    def pull(self):
        try:
            rx_raw = self.pull_raw()
            if rx_raw is None:
                return None
            msg_sets = self.raw_to_msg_sets(rx_raw)
            self.decode_message_sets(msg_sets)
            return msg_sets
        except PullDataTimedOutException:
            return None


    def raw_to_msg_sets(self, rx_raw):
        message_sets = rx_raw.split(BETWEEN_MESSAGE_SETS_SEP)[:-1]
        if len(message_sets) == 1:
            if BETWEEN_MESSAGE_SETS_SEP not in rx_raw:
                print('partial message data!')
                raise Exception
        #  print(len(message_sets))
        new_raw = None
        prev_raw = None
        message_sets_unfucked = []
        for message_set_raw in message_sets:
            if not message_set_raw.startswith(MESSAGE_SET_START):
                if MESSAGE_SET_START in message_set_raw:
                    try:
                        # not gonna lie I have no idea what this line is for anymore
                        message_set_raw = MESSAGE_SET_START+MESSAGE_SET_START.join(message_set_raw.split(MESSAGE_SET_START)[1:])
                    except Exception as e:
                        print(f'wtf: {e}')
                        print(f'message_set_raw: {message_set_raw}')
                        raise Exception
                else:
                    continue
            message_sets_unfucked.append(message_set_raw[3:])
        return message_sets_unfucked

    def decode_message_sets(self, msg_sets):
        for msg_set in msg_sets:
            try:
                self.data.decode_incoming(msg_set)
            except Exception as e:
                if f'{type(e)}' == "<class 'google.protobuf.message.DecodeError>'":
                    print(f"Failed to decode data: {e}")
                else:
                    print(f'unknown exception pulling data: {e}')

    
    def append_pb_vals(self):
        self.data.append_pb_vals()

    def append_cmd_vals(self):
        self.data.append_cmd_vals()
