import threading
import socket
import time
import os
import subprocess  # For executing a shell command
import datetime

from util import is_windows
from constants import (BETWEEN_MESSAGE_SETS_SEP, BETWEEN_MESSAGES_SEP, MESSAGE_SET_START)

COMMS_TIMEOUT = 8
PING_SILENT = True
#  MIN_CMD_DT = 0.1 #01
MIN_CMD_DT = 0.0000000001

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
        self.last_pushed = time.time()

        #  self.playback_dir = '/home/k/robot_data/20220324142841954'
        #  self.playback_dir = '/home/k/robot_data/20220324185127868'
        self.playback_base_dir = os.path.join(os.path.expanduser('~'), 'robot_data')
        self.playback_dirnames = None
        self.playback_dir_num = None
        self.playback_started = False  
        self.playback_filenames = None
        self.playback_file_num = 0
        self.regenerate_playback_filenames()

    @property
    def playback_dir(self):
        #  print(f'self.playback_dirnames[self.playback_dir_num]: {self.playback_dirnames[self.playback_dir_num]}')
        #  print(f'self.playback_dir_num: {self.playback_dir_num}')
        #  print(f'self.playback_dirnames: {self.playback_dirnames}')
        return self.playback_dirnames[self.playback_dir_num]

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

    def get_playback_filenames(self):
        #  print(f'self.playback_filenames: {self.playback_filenames}')
        return self.playback_filenames

    def regenerate_playback_filenames(self):
        if self.playback_dirnames is None:
            self.playback_dirnames = [os.path.join(self.playback_base_dir,f) for f in sorted(os.listdir(self.playback_base_dir))]
            self.playback_dir_num = len(self.playback_dirnames)-1
        self.playback_filenames = [os.path.join(self.playback_dir,f[:-3]) for f in sorted(os.listdir(self.playback_dir))]
        #  print(f'generated playback_filenames: {self.playback_filenames}')

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
                t = time.time()
                dt = t - self.last_pushed
                tx_raw = None
                if dt > MIN_CMD_DT:
                    tx_raw = self.push()
                    self.last_pushed = t
                received = self.pull()

                # convert to string / copy existing string for extremely weak thread safety
                dirname = f'{self.data.recording_to_dirname}'
                if dirname != 'None':
                    if not os.path.exists(dirname):
                        os.makedirs(dirname)
                    #  print(f'recording to: {dirname}')
                    (dt, micro) = datetime.datetime.now().strftime('%Y%m%d%H%M%S.%f').split('.')
                    dt = "%s%03d" % (dt, int(micro) / 1000)
                    title = f'{dt}_rx'
                    if received is not None:
                        pb_filename = os.path.join(dirname,title)
                        #  print(f'pb_filename: {pb_filename}')
                        #  print(f'exists: {os.path.exists(dirname)}')
                        with open(pb_filename, 'wb') as f:
                            f.write(received)
                    if tx_raw is not None:
                        title = f'{dt}_tx'
                        #  print(f'dirname: {dirname}')
                        #  print(f'title: {title}')
                        pb_filename = os.path.join(dirname,title)
                        with open(pb_filename, 'wb') as f:
                            f.write(tx_raw)

                #  if encoded_in is None:
                    #  print('Received "None" for data, probably just timed out above? Did we make it to this statement?')
            else:
                #  self.append_pb_vals()
                if self.playback_started:
                    print('hi')
                    if self.playback_filenames is None:
                        self.regenerate_playback_filenames()
                        self.playback_file_num = 0

                    increment = 0
                    tx_file = self.playback_filenames[self.playback_file_num]+'_tx'
                    #  print(tx_file)
                    if os.path.exists(tx_file):
                        print('found tx file')
                        f = open(tx_file, "rb")
                        tx_raw = f.read()
                        print(f'tx_raw: {tx_raw}')
                        self.data.cmd.pb.ParseFromString(tx_raw)

                        # TODO (for krishn????) DONT INCREMENT BEYOND HOW MANY TICKS WE ACTUALLY RECORDED
                        increment += 1
                        #  /home/k/robot_data/20220324142841954/
                    rx_file = self.playback_filenames[self.playback_file_num]+'_rx'
                    if os.path.exists(rx_file):
                        f = open(rx_file, "rb")
                        rx_raw = f.read()
                        msg_sets = self.raw_to_msg_sets(rx_raw)
                        self.decode_message_sets(msg_sets)
                        increment += 1
                    self.playback_file_num += increment
                    time.sleep(0.5)
                else:
                    pass
                    #  self.playback_filenames = None
                    #  self.playback_file_num = 0

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
            raw_outgoing = self.data.encode_outgoing()
            encoded = raw_outgoing + BETWEEN_MESSAGES_SEP
            #  encoded2 = BETWEEN_MESSAGES_SEP + raw_outgoing + BETWEEN_MESSAGES_SEP
            print('encoded:')
            print(encoded)
            self.socket.sendall(encoded)
            return raw_outgoing
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
                return
            msg_sets = self.raw_to_msg_sets(rx_raw)
            self.decode_message_sets(msg_sets)
            return rx_raw
        except Exception as e:
            print(f'error while pulling data: {e}')


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
