import pygame as pg
import functools
import math

from size_constants import *
from tile import Tile
from segment import CornerCircle
from segment import Line

TILE_COLOUR_1 = (60,60,60)
TILE_COLOUR_2 = (30,100,100)

class Arena():
    def __init__(self, robot):
        self.rect = pg.Rect(0,0,ARENA_SIZE_PIXELS,ARENA_SIZE_PIXELS)
        self.robot = robot
        self.tiles = self.generate_tiles()
        self.segments = self.generate_segments()
        
        self.image = self.generate_image()
        self.active = self.segments[0] # active segment

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

    def render_active(self, screen):
        self.active.render(screen)

        # erase/re-render previously active
        idx = self.segments.index(self.active)
        if idx>0:
            self.segments[idx-1].render(screen)

    def generate_segments(self):
        segments = [
            Line(start=(3.5,5.5), end=(1,5.5), active=True),
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
            CornerCircle(center=(1,1),corner=Diagonal.TL),
            Line(start=(3,2.5), end=(3.5,2.5))
        ]
        return segments

