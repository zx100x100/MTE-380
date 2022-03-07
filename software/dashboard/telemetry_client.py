import threading
import socket
import time

COMMS_TIMEOUT = 4

class TelemetryClient(threading.Thread):
    def __init__(self, data, server_ip, server_port):
        super().__init__()
        self.data = data
        self.connected = False
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_ip = server_ip
        self.server_port = server_port
        self.killme = False

    def kill_thread(self):
        self.killme = True

    def run(self):
        longest_pull = 0
        longest_push = 0
        while True:
            if self.killme:
                break
            if self.connected:
                before_tick = time.time()
                self.push()
                new_time = time.time()
                push_time = new_time - before_tick
                #  time.sleep(0.1)
                self.pull()
                new_time2 = time.time()
                pull_time = new_time2 - new_time
                if pull_time > longest_pull:
                    longest_pull = pull_time
                if push_time > longest_push:
                    longest_push = push_time

                #  print(f'push_time: {push_time}')
                #  print(f'pull_time: {pull_time}')
                #  print(f'longest_push: {longest_push}')
                #  print(f'longest_pull: {longest_pull}')
            else:
                self.pull_fake()
                time.sleep(0.1)

    def connect(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(COMMS_TIMEOUT)
            print(self.server_ip)
            print(self.server_port)

            self.socket.connect((self.server_ip,self.server_port))
            self.socket.settimeout(None)
            self.connected = True

        except Exception as e:
            print(f'error connecting: {e}')
            self.connected = False
            try:
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.socket.settimeout(COMMS_TIMEOUT)
                self.socket.connect((self.server_ip,self.server_port))
                self.socket.settimeout(None)
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
            sys.exit()
        print('disconnected')
        sys.exit()
        self.connected = False

    def push(self):
        try:
            #  print('push')
            self.socket.sendall(self.data.encode_outgoing())
        except Exception as e:
            print(f"Failed to push data: {e}")
            print(f"Attempting to reconnect.")
            self.connect()
            #  self.controls.connect_button.refresh_state()

    def pull(self):
        try:
            #  print('pull')
            rx_raw = self.socket.recv(1000)

            message_sets = rx_raw.split(b';;;')[:-1]
            print(len(message_sets))
            for message_set_raw in message_sets:
                self.data.decode_incoming(message_set_raw)
        except Exception as e:
            print(f"Failed to pull data: {e}")
    
    def pull_fake(self):
        self.data.pull_fake()
