import time
import os
import pygame as pg

WIDTH = 100
HEIGHT = 700
FILENAME_L_MARGIN = 2
FILENAME_L_PAD = 1
FILENAME_HEIGHT = 12
FONTSIZE = 10
TITLE_FONTSIZE = 16
TITLE_HEIGHT = 35
TRIANGLE_WIDTH = 15
TRIANGLE_HEIGHT = 30
TRIANGLE_COLOUR = (255,0,255)
FONT_COLOUR = (255,255,255)
FILENAME_TOP_MARGIN = 10
TRIANGLES_MARGIN = 10

class Playback:
    def __init__(self, app):
        self.app = app
        screen_rect = self.app.screen.get_rect()
        right = screen_rect.right
        self.rect = pg.Rect(right - WIDTH,0,WIDTH, HEIGHT)
        self.filename.rect = pg.Rect(FILENAME_L_MARGIN,0,WIDTH, HEIGHT)
        self.bg_image = self.generate_bg_image()
        self.filename_font = pg.font.SysFont('Arial', FONTSIZE)
        self.title_font = pg.font.SysFont('Arial', TITLE_FONTSIZE)
        self.right_triangle_image, self.left_triangle_image = self.generate_triangle_images()

        self.prev_file_rect = pg.Rect(0,TITLE_HEIGHT,TRIANGLE_WIDTH,TRIANGLE_HEIGHT)
        self.next_file_rect = pg.Rect(self.rect.width-TRIANGLE_WIDTH,TITLE_HEIGHT,TRIANGLE_WIDTH,TRIANGLE_HEIGHT)
        self.prev_tick_rect = pg.Rect(0,self.prev_file_rect.bottom+TRIANGLES_MARGIN,TRIANGLE_WIDTH,TRIANGLE_HEIGHT)
        self.next_tick_rect = pg.Rect(self.rect.width-TRIANGLE_WIDTH,self.prev_file_rect.bottom+TRIANGLES_MARGIN,TRIANGLE_WIDTH,TRIANGLE_HEIGHT)

    def handle_click(self, pos):
        x = pos[0] - self.rect.left
        y = pos[1]

        if (x < self.rect.left or y > self.rect.bottom):
            return

        if util.pos_inside_rect((x,y), self.prev_file_rect):
            self.prev_file()
            return
        if util.pos_inside_rect((x,y), self.next_file_rect):
            self.next_file()
            return
        if util.pos_inside_rect((x,y), self.prev_tick_rect):
            self.prev_tick()
            return
        if util.pos_inside_rect((x,y), self.next_tick_rect):
            self.next_tick()
            return
    
    def prev_file(self):
        if 

    def prev_file(self):
        self.telemetry_client.playback_started = False
        time.sleep(0.35)
        self.telemetry_client.playback

            

    def generate_triangle_images(self):
        image = pg.Surface(TRIANGLE_WIDTH, TRIANGLE_HEIGHT)
        image.fill((0,0,0,0))
        pg.draw.polygon(image, TRIANGLE_COLOUR, ((0,0),(TRIANGLE_WIDTH,TRIANGLE_HEIGHT/2),(0,TRIANGLE_HEIGHT)))
        return image, pg.transform.flip(image, False, True)

    def generate_filename_image(self):
        image = pg.Surface((self.rect.width #FILENAME_HEIGHT

    def generate_bg_image(self):
        image = pg.Surface(self.rect.size)
        image.fill((100,100,100))

        title_surf = self.title_font.render('Playback', True, FONT_COLOUR)
        image.blit(title_surf, (10, 10))

        image.blit(self.left_triangle_image, self.prev_file_rect)
        image.blit(self.left_triangle_image, self.prev_tick_rect)
        image.blit(self.right_triangle_image, self.next_file_rect)
        image.blit(self.right_triangle_image, self.next_tick_rect)
        return image





