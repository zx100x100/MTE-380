import socket
import time

from proto.nav_data_pb2 import (NavData)
from proto.cmd_data_pb2 import (CmdData)
                           
from proto.imu_data_pb2 import ImuData
from proto.tof_data_pb2 import TofData
from proto.guidance_data_pb2 import GuidanceData


HOST = "192.168.100.111"  # The server's hostname or IP address
PORT = 23  # The port used by the server

#  pos = Nav()
#  pos.loc_x = 1.0
#  pos.loc_y = 1.0
#  serialized = pos.SerializeToString()
#  print(serialized)
#  pos2 = Nav()
#  pos2.ParseFromString(serialized)
#  print(f'pos decoded: {pos2.loc_x}')

cmd = CmdData()
cmd.runState = CmdData.RunState.STOP
cmd.teleop.leftPower = 1.0
cmd.teleop.rightPower = 1.0

cmd_serialized = cmd.SerializeToString()
print(f'cmd_serialized: {cmd_serialized}')

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print('connected')
    while True:
        try:
            s.sendall(cmd_serialized)
            print("Receiving:")
            rx_raw = s.recv(300)
            msgs = [
                NavData(),
                GuidanceData(),
                ImuData()
            ] + [TofData() for i in range(4)]
            #  msgs = [NavData()]

            #  bytes_decoded = 0
            #  total_to_decode = sum([i.ByteSize() for i in msgs])
            #  print(total_to_decode)
            #  print(len(rx_raw))
            print(rx_raw)
            #  print(cmd_data.teleop.leftPower)
            for raw_data, msg in zip(rx_raw.split(b':'),msgs):
                msg.ParseFromString(raw_data)

            print(msgs[0].posX)
            print(msgs[5].dist)


        except Exception as e:
            print(f'e: {e}')

        time.sleep(0.1)

