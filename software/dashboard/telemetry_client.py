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

    def run(self):
        while True:
            if self.connected:
                self.push()
                self.pull()
                time.sleep(0.01)
            else:
                self.pull_fake()
                time.sleep(0.1)

    def connect(self):
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(COMMS_TIMEOUT)
            self.socket.connect((self.server_ip,self.server_port))
            self.socket.settimeout(None)
            self.connected = True

        except Exception as e:
            print(f'error connecting: {e}')
            self.connected = False
            try:
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                self.socket.settimeout(COMMS_TIMEOUT)
                self.socket.settimeout(None)
                self.connected = True
            except Exception as e:
                print(f'error connecting AGAIN! {e}')
                self.connected = False

    def disconnect(self):
        try:
            self.socket.shutdown(socket.SHUT_RDWR)
            self.socket.close()
        except Exception as e:
            print(f"Failed to disconnect comms: {e}")
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.connected = False

    def push(self):
        try:
            #  print('push')
            self.socket.sendall(self.data.encode_outgoing())
        except Exception as e:
            print(f"Failed to push data: {e}")

    def pull(self):
        try:
            #  print('pull')
            rx_raw = self.socket.recv(300)
            #  print('rec')
            self.data.decode_incoming(rx_raw)
        except Exception as e:
            print(f"Failed to pull data: {e}")
    
    def pull_fake(self):
        self.data.pull_fake()
