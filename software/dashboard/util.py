import numpy as np
import pygame as pg
import os

def is_windows():
    return os.name == 'nt'

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

def pos_inside_rect(pos, rect):
    if pos[0] > rect.right or pos[0] < rect.left:
        return False
    if pos[1] > rect.bottom or pos[1] < rect.top:
        return False
    return True

def next_enum(enum_obj):
    cls = enum_obj.__class__
    members = list(cls)
    index = members.index(enum_obj) + 1
    if index >= len(members):
        # to cycle around
        # index = 0
        #
        # to error out
        raise StopIteration('end of enumeration reached')
    return members[index]

def prev_enum(enum_obj):
    cls = enum_obj.__class__
    members = list(cls)
    index = members.index(enum_obj) - 1
    if index < 0:
        # to cycle around
        # index = len(members) - 1
        #
        # to error out
        raise StopIteration('beginning of enumeration reached')
    return members[index]
