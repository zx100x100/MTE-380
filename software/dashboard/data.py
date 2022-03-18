from proto.nav_data_pb2 import (NavData)
from proto.imu_data_pb2 import ImuData
from proto.tof_data_pb2 import TofData
from proto.guidance_data_pb2 import GuidanceData
from proto.hms_and_cmd_data_pb2 import (HmsData, CmdData)

from protobuf_readouts import ProtobufReadouts

N_TRAPS = 8

class PbData:
    def __init__(self, pb):
        self.pb = pb
        self.readout = None

    def set_readout(self, readout):
        self.readout = readout

    def parse_new(self, raw):
        self.pb.ParseFromString(raw)
        self.readout.update_vals()
    
    # pull value without decoding new one
    # for testing
    def update_vals(self):
        self.readout.update_vals()
        #  self.readout.items[0].values[-1] = self.readout.items[0].values[-2]+0.01
        #  self.readout.items[0].values[-1] = 0.1 * (self.readout.items[0].values[-2]/abs(self.readout.items[0].values[-2]))*-1

class Data:
    def __init__(self, app):
        self.app = app
        self.cmd = PbData(CmdData())
        self.cmd.pb.runState = CmdData.RunState.E_STOP
        #  for i in range(len(self.cmd.pb.trapX)):
            #  self.cmd.pb.trapX[i] = self.cmd.pb.trapY[i] = -1
        self.cmd.pb.trapX.extend([-1] * N_TRAPS)
        self.cmd.pb.trapY.extend([-1] * N_TRAPS)
        self.nav = PbData(NavData())
        self.guidance = PbData(GuidanceData())
        self.hms = PbData(HmsData())
        self.imu = PbData(ImuData())
        self.tof = [PbData(TofData()) for i in range(4)]
        self.incoming = [self.nav,
                         self.guidance,
                         self.hms,
                         self.imu] + self.tof
        readout_columns = [[self.cmd],
                           [self.nav],
                           [self.guidance],
                           [self.hms],
                           [self.imu],
                           self.tof]
        self.readouts = ProtobufReadouts(self.app, readout_columns)


    def encode_outgoing(self):
        out = self.cmd.pb.SerializeToString()
        #  print(f'out: {out}')
        #  print(out)
        return out

    def decode_incoming(self, raw):
        #  print(f'raw: {raw}')
        for raw_msg, msg in zip(raw.split(b':::'),self.incoming):
            msg.parse_new(raw_msg)

        #  print(f'self.nav.pb.posX: {self.nav.pb.posX}')
        #  print(f'self.imu.pb.gyroZ: {self.imu.pb.gyroZ}')
    
    # read the same value into the data array when disconnected so i can test my dang plots
    def append_cmd_vals(self):
        self.cmd.update_vals()

    def append_pb_vals(self):
        for msg in self.incoming:
            msg.update_vals() # just push the same value without reading a new one

        self.cmd.update_vals()
