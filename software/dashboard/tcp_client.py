import socket
import time

from proto.position_pb2 import (Position)
from proto.command_pb2 import (Command,
                               TeleopCmd,
                               RobotState,
                               RunState)


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

command = Command()
#  run_state = RunState.STOP
#  command.robot_state.run_state = run_state
#  command.teleop_cmd.left_power = 1.0
#  command.teleop_cmd.right_power = 1.0
command.test = 1.0

command_serialized = command.SerializeToString()

print(f'command_serialized: {command_serialized}')

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print('connected')
    while True:
        try:
            print("Receiving:")
            str_data = s.recv(60)
            position = Position()
            #  print(f'equal? {str_data == serialized}')
            print(f"Received {str_data}")
            position.ParseFromString(str_data)
        except Exception as e:
            print(f'e: {e}')

        #  print(f'position: {position.loc_x}')
        #  print(f'acc: {position.acc_x}')
        #  print(f'position: {position.loc_x} {position.loc_y}')
        s.sendall(command_serialized)
        time.sleep(1)

