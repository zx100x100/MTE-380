import pygame as pg
import numpy as np

from constants import *
ROBOT_COLOUR = (0,0,0,60)
ROBOT_HEADING_COLOUR = (0,255,0)
ROBOT_HEADING_ARC_COLOUR = (200,0,200)


default_pos = (3.5,5.5)

class Robot():
    def __init__(self):
        self.angle = 180
        self.image = self.generate_image()
        self.rect = self.image.get_rect()
        self.rect.centerx = int(default_pos[0] * PIXELS_PER_TILE)
        self.rect.centery = int(default_pos[1] * PIXELS_PER_TILE)
        self.angle_error = 0
        self.update_sprite_angle()

    def generate_image(self):
        image = pg.Surface((ROBOT_SIZE_PIXELS[0],ROBOT_SIZE_PIXELS[1])).convert_alpha()

        image.fill(ROBOT_COLOUR)
        rect = image.get_rect()
        pg.draw.line(image, ROBOT_HEADING_COLOUR,
                     rect.center, (rect.right, rect.centery),3)
        return image

    def update_sprite_angle(self):
        self.angled_image = pg.transform.rotate(self.image,
                                        -self.angle)
        self.angled_rect = self.angled_image.get_rect()
        arc_rect = pg.Rect(0,0,30,30)
        arc_rect.center = self.angled_rect.center
        start_angle = np.deg2rad(360-(self.angle%360))
        end_angle = np.deg2rad(360)
        pg.draw.arc(self.angled_image,ROBOT_HEADING_ARC_COLOUR,arc_rect,start_angle,end_angle,3)
        self.angled_rect.center = self.rect.center
        #  print(f'robot center: {self.rect.center}')

    def render(self, screen):
        screen.blit(self.angled_image, self.angled_rect)
