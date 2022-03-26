from enum import Enum
import numpy as np
from proto.hms_and_cmd_data_pb2 import CmdData
import os


KP_VEL = 140.0
KI_VEL = 0.001
KD_VEL = 0.0
KP_DRIFT = 130.0
KI_DRIFT = 0.001
KD_DRIFT = 40.0

N_TRAPS = 8
#  MAX_DATA_POINTS = 100
PLOT_SIZE = (200,140) # pixels for the size of the inner plot
DISPLAY_DATA_POINTS = PLOT_SIZE[0]

UGLY_SQUEEZE_MODE = True
if os.path.exists(os.path.join(os.getcwd(),'.squeeze_ui')):
    print('using ugly squeeze mode for kaelans tiny laptop screen')
    UGLY_SQUEEZE_MODE = True

if UGLY_SQUEEZE_MODE:
    SCREEN_SIZE = (1368,710)
else:
    SCREEN_SIZE = (1600,710)
ARENA_SIZE_INCHES = 72
PIXELS_PER_INCH = 5
INCHES_PER_TILE = 12
PIXELS_PER_TILE = INCHES_PER_TILE * PIXELS_PER_INCH
ARENA_SIZE_PIXELS = PIXELS_PER_INCH * ARENA_SIZE_INCHES
ROBOT_SIZE_INCHES = (8,7.2)
ROBOT_SIZE_PIXELS = (int(round(ROBOT_SIZE_INCHES[0]*PIXELS_PER_INCH)),int(round(ROBOT_SIZE_INCHES[1]*PIXELS_PER_INCH)))

CONTROL_ITEM_HEIGHT = 30
CONTROL_ITEM_WIDTH = 160
GLOBAL_MARGIN = 8

STOPPED_STATES = [CmdData.RunState.E_STOP]
STARTED_STATES = [CmdData.RunState.AUTO, CmdData.RunState.TELEOP, CmdData.RunState.SIM]
            
BETWEEN_MESSAGE_SETS_SEP = b';;;'
BETWEEN_MESSAGES_SEP = b'xxx'
MESSAGE_SET_START = b'((('


class TRBL(Enum): # helper because I never learned how to count
    T = 0 # top
    R = 1 # right
    B = 2 # bottom
    L = 3 # left (just like CSS)

class Diagonal(Enum): # for corner circles
    TL = 0 # top left
    TR = 1 # top right
    BL = 2 # bottom left
    BR = 3 # bottom right

class TofPosition(Enum):
    FRONT = 0
    LEFT_FRONT = 1
    LEFT_REAR = 2
    REAR = 3

