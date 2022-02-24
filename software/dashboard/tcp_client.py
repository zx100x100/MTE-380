import socket
import time

from proto.nav_data_pb2 import (NavData)
from proto.cmd_data_pb2 import (CmdData,
                           Teleop,
                           RunState)
from proto.imu_data import ImuData
from proto.tof_data import TofData
from proto.guidance_data import GuidanceData


HOST = "192.168.86.111"  # The server's hostname or IP address
PORT = 23  # The port used by the server

#  pos = Nav()
#  pos.loc_x = 1.0
#  pos.loc_y = 1.0
#  serialized = pos.SerializeToString()
#  print(serialized)
#  pos2 = Nav()
#  pos2.ParseFromString(serialized)
#  print(f'pos decoded: {pos2.loc_x}')

cmd = Cmd()
run_state = RunState.STOP
cmd.run_state = RunState.STOP
cmd.teleop.left_power = 1.0
cmd.teleop.right_power = 1.0

cmd_serialized = cmd.SerializeToString()

print(f'cmd_serialized: {cmd_serialized}')

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print('connected')
    while True:
        try:
            print("Receiving:")
            str_data = s.recv(300)
            decoded = [
                NavData(),
                GuidanceData(),
                CmdData(),
                ImuData()
            ] + [TofData() for i in range 4]

            print(f"Received {str_data}")
            blocks = str_data.split('|||')
            for data_block, data_obj in zip(blocks, decoded):
                data_obj.ParseFromString(data_block)
            print(decoded[0].posX)
        except Exception as e:
            print(f'e: {e}')

        s.sendall(cmd_serialized)
        time.sleep(1)

