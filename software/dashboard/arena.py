import pygame as pg
import functools
import math

from constants import *
from tile import Tile
from segment import CornerCircle
from segment import Line
from util import pos_inside_rect

TILE_COLOUR_1 = (60,60,60)
TILE_COLOUR_2 = (30,100,100)
TRAP_TILE_COLOUR = (255,0,0,50)

DISABLE_VEL_SETPOINT_INDICATORS = False

class Arena():
    def __init__(self, robot, app):
        self.rect = pg.Rect(0,0,ARENA_SIZE_PIXELS,ARENA_SIZE_PIXELS)
        self.app = app
        self.robot = robot
        self.tiles = self.generate_tiles()
        self.segments = self.generate_segments()
        
        self.image = self.generate_image()
        self.active = self.segments[0] # active segment
        self.need_re_render = False

    def erase(self, screen):
        screen.blit(self.image, (0,0))


    def generate_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill(TILE_COLOUR_1)
        rect = image.get_rect()
        for row in range(6):
            for col in range(6):
                if row % 2 == col % 2:
                    pg.draw.rect(image, TILE_COLOUR_2, pg.Rect(col* PIXELS_PER_TILE, row * PIXELS_PER_TILE, PIXELS_PER_TILE, PIXELS_PER_TILE))
                if self.tiles[row][col].trap:
                    pg.draw.rect(image, TRAP_TILE_COLOUR, pg.Rect(col* PIXELS_PER_TILE, row * PIXELS_PER_TILE, PIXELS_PER_TILE, PIXELS_PER_TILE))

        # temp deactivate
        self.segments[0].active = False
        for segment in self.segments:
            segment.render(image)
        self.segments[0].active = True

        return image

    def generate_tiles(self):
        tiles = []
        for row in range(6):
            new_row = []
            for col in range(6):
                new_row.append(Tile(row, col))
            tiles.append(new_row)
        return tiles

    def reset_vel_setpoint_indicators(self):
        for s in self.segments:
            s.reset_vel_setpoint_lines()

    def generate_vel_setpoint_indicator(self, vel_setpoint):
        if not DISABLE_VEL_SETPOINT_INDICATORS:
            #  print(f'sp: {vel_setpoint}, pos: {self.robot.rect.center}')
            #  import time
            #  time.sleep(0.2)
            self.active.add_vel_setpoint_indicator_line(self.robot.rect.center, vel_setpoint)
    
    def set_active_segment(self, idx):
        self.active.active = False
        self.active = self.segments[idx]
        self.active.active = True

    def update_active_segment(self):
        if self.active.completed(self.robot.rect.center):
            idx = self.segments.index(self.active)
            self.active.active = False
            if idx+1>=len(self.segments):
                self.active = self.segments[0]
                #  pass
            else:
                self.active = self.segments[idx+1]
            self.active.active = True

    def re_render_tiles_to_refresh_traps_if_needed(self, screen):
        if self.need_re_render:
            self.image = self.generate_image()
            self.need_re_render = False
            screen.blit(self.image, (0,0))

    def render_active(self, screen):
        self.re_render_tiles_to_refresh_traps_if_needed(screen)
        self.active.render(screen)

        # erase/re-render previously active
        idx = self.segments.index(self.active)
        if idx>0:
            self.segments[idx-1].render(screen)

    def generate_segments(self):
        segments = [
            Line(start=(4.5,5.5), end=(1,5.5), active=True),
            CornerCircle(center=(1,5),corner=Diagonal.BL),
            Line(start=(0.5,5), end=(0.5,1)),
            CornerCircle(center=(1,1),corner=Diagonal.TL),
            Line(start=(1,0.5), end=(5,0.5)),
            CornerCircle(center=(5,1),corner=Diagonal.TR),
            Line(start=(5.5,1), end=(5.5,4)),
            CornerCircle(center=(5,4),corner=Diagonal.BR),
            Line(start=(5,4.5), end=(2,4.5)),
            CornerCircle(center=(2,4),corner=Diagonal.BL),
            Line(start=(1.5,4), end=(1.5,2)),
            CornerCircle(center=(2,2),corner=Diagonal.TL),
            Line(start=(2,1.5), end=(4,1.5)),
            CornerCircle(center=(4,2),corner=Diagonal.TR),
            Line(start=(4.5,2), end=(4.5,3)),
            CornerCircle(center=(4,3),corner=Diagonal.BR),
            Line(start=(4,3.5), end=(3,3.5)),
            CornerCircle(center=(3,3),corner=Diagonal.BL),
            CornerCircle(center=(3,3),corner=Diagonal.TL),
            Line(start=(3,2.5), end=(3.5,2.5))
        ]
        return segments

    def handle_click(self, pos):
        if pos_inside_rect(pos, self.rect):
            for row in self.tiles:
                for tile in row:
                    if pos_inside_rect(pos, tile.rect):
                        tile.trap = not tile.trap
                        self.need_re_render = True
                        return (tile.col, tile.row)

        return False

