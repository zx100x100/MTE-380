import pygame as pg

from constants import *

class Tile:
    def __init__(self, row, col):
        self.row = row
        self.col = col
        self.rect = pg.Rect(row*PIXELS_PER_TILE,
                         col*PIXELS_PER_TILE,
                         PIXELS_PER_TILE,
                         PIXELS_PER_TILE)
