import socket

<<<<<<< HEAD
HOST = "192.168.86.111"  # The server's hostname or IP address
PORT = 23  # The port used by the server

while True:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(b"Hello, world")
        data = s.recv(1024)

    print(f"Received {data!r}")
=======
HOST = "192.168.1.111"  # The server's hostname or IP address
PORT = 23  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b"Hello, world")
    data = s.recv(1024)

print(f"Received {data!r}")
>>>>>>> 1b02b63bb6d6ec40731a0fc67adaf2662be04a12
