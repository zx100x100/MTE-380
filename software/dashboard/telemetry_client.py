import threading
import socket
import time
from constants import (BETWEEN_MESSAGE_SETS_SEP, BETWEEN_MESSAGES_SEP, MESSAGE_SET_START)

COMMS_TIMEOUT = 6

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

    def kill_thread(self):
        self.killme = True
    
    def disconnect_when_convenient(self):
        self.disconnectme = True

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
                before_tick = time.time()
                encoded_out = self.push()
                new_time = time.time()
                push_time = new_time - before_tick
                #  time.sleep(0.1)
                encoded_in = self.pull()
                
                if self.data.recording_to_dirname is not None:
                    print('writing data')
                    if encoded_in is None:
                        encoded_in = b'None'
                    elif encoded_out is None:
                        encoded_out = b'None'
                    self.data.append_pb_data_to_file(data_received=encoded_in, data_sent=encoded_out)
                new_time2 = time.time()
                pull_time = new_time2 - new_time
                if pull_time > longest_pull:
                    longest_pull = pull_time
                if push_time > longest_push:
                    longest_push = push_time

                #  print(f'longest_pull: {longest_pull}')

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

    def pull(self):
        try:
            #  print('pull')
            rx_raw = self.socket.recv(1000)

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
        except Exception as e:
            if type(e) is socket.timeout:
                print('Connection timed out!!! Disconnecting')
                self.connected = False
            elif f'{type(e)}' == "<class 'google.protobuf.message.DecodeError>'":
                print(f"Failed to decode data: {e}")
            else:
                print(f'unknown exception pulling data: {e}')

            return None
        self.data.decode_incoming()
        prev_raw = message_set_raw
        return rx_raw

    
    def append_pb_vals(self):
        self.data.append_pb_vals()

    def append_cmd_vals(self):
        self.data.append_cmd_vals()
