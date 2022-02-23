import socket

from proto.position_pb2 import Position

HOST = "192.168.86.111"  # The server's hostname or IP address
PORT = 23  # The port used by the server

        
#  pos = Position()
#  pos.loc_x = 1.0
#  pos.loc_y = 1.0
#  serialized = pos.SerializeToString()
#  print(serialized)
#  pos2 = Position()
#  pos2.ParseFromString(serialized)
#  print(f'pos decoded: {pos2.loc_x}')

while True:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(b"Hello, world")
        data = s.recv(1024)

        position = Position()
        try:
            str_data = data
            #  print(f'equal? {str_data == serialized}')
            #  print(str_data)
            #  print(serialized)
            position.ParseFromString(str_data)
        except Exception as e:
            print(f'e: {e}')

        print(f'position: {position.loc_x}')
        print(f'acc: {position.acc_x}')
        #  print(f'position: {position.loc_x} {position.loc_y}')

    print(f"Received {data!r}")
