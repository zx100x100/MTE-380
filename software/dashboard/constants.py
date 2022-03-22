from enum import Enum
import numpy as np
from proto.hms_and_cmd_data_pb2 import CmdData


KP_VEL = 100.0
KI_VEL = 0.0
KD_VEL = 0.0
KP_DRIFT = 150.0
KI_DRIFT = 0.0
KD_DRIFT = 180.0

N_TRAPS = 8
#  MAX_DATA_POINTS = 100
PLOT_SIZE = (200,140) # pixels for the size of the inner plot
DISPLAY_DATA_POINTS = PLOT_SIZE[0]
SCREEN_SIZE = (1366,710)
ARENA_SIZE_INCHES = 72
PIXELS_PER_INCH = 5
INCHES_PER_TILE = 12
PIXELS_PER_TILE = INCHES_PER_TILE * PIXELS_PER_INCH
print(f'PIXELS_PER_TILE: {PIXELS_PER_TILE}')
ARENA_SIZE_PIXELS = PIXELS_PER_INCH * ARENA_SIZE_INCHES
ROBOT_SIZE_INCHES = (8,7.2)
ROBOT_SIZE_PIXELS = (int(round(ROBOT_SIZE_INCHES[0]*PIXELS_PER_INCH)),int(round(ROBOT_SIZE_INCHES[1]*PIXELS_PER_INCH)))
print(f'robot_SIZE_PIXELS: {ROBOT_SIZE_PIXELS}')
print(f'SCREEN_SIZE: {SCREEN_SIZE}')

CONTROL_ITEM_HEIGHT = 30
CONTROL_ITEM_WIDTH = 160
GLOBAL_MARGIN = 8

STOPPED_STATES = [CmdData.RunState.E_STOP]
STARTED_STATES = [CmdData.RunState.AUTO, CmdData.RunState.TELEOP, CmdData.RunState.SIM]
            
BETWEEN_MESSAGE_SETS_SEP = b';;;'
BETWEEN_MESSAGES_SEP = b':::'
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
