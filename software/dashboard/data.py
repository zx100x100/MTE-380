from proto.nav_data_pb2 import (NavData)
from proto.imu_data_pb2 import ImuData
from proto.tof_data_pb2 import TofData
from proto.guidance_data_pb2 import GuidanceData
from proto.hms_and_cmd_data_pb2 import (HmsData, CmdData)
from protobuf_readouts import ProtobufReadouts
import constants
from constants import (BETWEEN_MESSAGE_SETS_SEP, BETWEEN_MESSAGES_SEP, MESSAGE_SET_START)

import datetime


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
        self.cmd.pb.trapX.extend([-1] * constants.N_TRAPS)
        self.cmd.pb.trapY.extend([-1] * constants.N_TRAPS)
        self.cmd.pb.kP_vel = constants.KP_VEL
        self.cmd.pb.kI_vel = constants.KI_VEL
        self.cmd.pb.kD_vel = constants.KD_VEL
        self.cmd.pb.kP_drift = constants.KP_DRIFT
        self.cmd.pb.kI_drift = constants.KI_DRIFT
        self.cmd.pb.kD_drift = constants.KD_DRIFT
        self.cmd.pb.guidanceLogLevel = HmsData.LogLevel.OVERKILL
        self.nav = PbData(NavData())
        self.guidance = PbData(GuidanceData())
        self.hms = PbData(HmsData())
        self.imu = PbData(ImuData())
        self.tof = [PbData(TofData()) for i in range(4)]
        self.recording_to_dirname = None
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

    def generate_recording_filename(self):
        (dt, micro) = datetime.datetime.now().strftime('%Y%m%d%H%M%S.%f').split('.')
        dt = "%s%03d" % (dt, int(micro) / 1000)
        return str(dt)

    def append_pb_data_to_file(self, data_received, data_sent):
        if self.recording_to_dirname is None:
            print('!!!!!!!!!!!!!!!')
            return
        new_filename = self.generate_recording_filename()
        print(f'self.recording_to_dirname: {self.recording_to_dirname}')
        rec_filename = self.recording_to_dirname+'/'+'rec_'+new_filename
        print(f'rec_filename: {rec_filename}')
        with open(rec_filename, 'ab') as f:
            f.write(data_received)
        with open(self.recording_to_dirname+'/'+'cmd_'+new_filename, 'ab') as f:
            f.write(data_sent)



    def encode_outgoing(self):
        out = self.cmd.pb.SerializeToString()
        #  print(f'out: {out}')
        #  print(out)
        return out

    def decode_incoming(self, raw):
        #  print(f'raw: {raw}')
        for raw_msg, msg in zip(raw.split(BETWEEN_MESSAGES_SEP),self.incoming):
            msg.parse_new(raw_msg)
        #  print(f'self.imu.pb.gyroZ: {self.imu.pb.gyroZ}')
    
    # read the same value into the data array when disconnected so i can test my dang plots
    def append_cmd_vals(self):
        self.cmd.update_vals()

    def append_pb_vals(self):
        for msg in self.incoming:
            msg.update_vals() # just push the same value without reading a new one

        self.cmd.update_vals()
