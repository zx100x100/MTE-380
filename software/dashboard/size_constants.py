from enum import Enum
import numpy as np

SCREEN_SIZE = (1366,768)
ARENA_SIZE_INCHES = 72
PIXELS_PER_INCH = 10
INCHES_PER_TILE = 12
PIXELS_PER_TILE = INCHES_PER_TILE * PIXELS_PER_INCH
print(f'PIXELS_PER_TILE: {PIXELS_PER_TILE}')
ARENA_SIZE_PIXELS = PIXELS_PER_INCH * ARENA_SIZE_INCHES
ROBOT_SIZE_INCHES = (8,7.2)
ROBOT_SIZE_PIXELS = (int(round(ROBOT_SIZE_INCHES[0]*PIXELS_PER_INCH)),int(round(ROBOT_SIZE_INCHES[1]*PIXELS_PER_INCH)))
print(f'robot_SIZE_PIXELS: {ROBOT_SIZE_PIXELS}')

def pixels_to_tiles(pixel_loc):
    return (pixel_loc[0]/PIXELS_PER_TILE, pixel_loc[1]/PIXELS_PER_TILE)

def tiles_to_pixels(tile_loc):
    return (tile_loc[0]*PIXELS_PER_TILE, tile_loc[1]*PIXELS_PER_TILE)

def angle_between_positions(pos1, pos2):
    # positive angle from x axis going clockwise to position in our weird inverted y axis
    x = pos2[0]-pos1[0]
    y = pos2[1]-pos1[1]

    theta = np.rad2deg(np.arctan2(y,x))
    if theta < 360:
        theta += 360
    return theta % 360

def angle_to_pg_angle(angle):
    # counterclockwise angle from x in degrees (our angle syste) to pygame's definition of an angle which is ccw from x in rad
    return np.deg2rad(360-(angle % 360))

#  print(f'-1,-1(225?): {vector_angle_between_positions((0,0),(-1,-1))}')
#  print(f'-1,1(135?): {vector_angle_between_positions((0,0),(-1,1))}')
#  print(f'1,1(45?): {vector_angle_between_positions((0,0),(1,1))}')

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
