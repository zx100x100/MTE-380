import time
import os
import pygame as pg

import util

WIDTH = 140
HEIGHT = 300
DIRNAME_L_PAD = 1
DIRNAME_HEIGHT = 12
FILENAME_HEIGHT = 12
FONTSIZE = 10
TITLE_FONTSIZE = 16
TITLE_HEIGHT = 35
TRIANGLE_WIDTH = 15
DIRNAME_L_MARGIN = 2
TRIANGLE_HEIGHT = 30
TRIANGLE_COLOUR = (255,0,255)
FONT_COLOUR = (255,255,255)
DIRNAME_TOP_MARGIN = 10
FILENAME_TOP_MARGIN = 18
TRIANGLES_MARGIN = 10

DIRNAME_WIDTH = WIDTH - 2 * TRIANGLE_WIDTH - DIRNAME_L_MARGIN

BG_GREY = (100,100,100)

class Playback:
    def __init__(self, app):
        self.app = app
        screen_rect = self.app.screen.get_rect()
        right = screen_rect.right
        self.last_dirname = None
        self.last_filename = None
        self.rect = pg.Rect(right - WIDTH,0,WIDTH, HEIGHT)
        self.dirname_rect = pg.Rect(TRIANGLE_WIDTH+DIRNAME_L_MARGIN,0,DIRNAME_WIDTH,DIRNAME_HEIGHT)
        self.filename_rect = self.dirname_rect.move(0,50)
        self.dirname_font = pg.font.SysFont('Arial', FONTSIZE)
        self.title_font = pg.font.SysFont('Arial', TITLE_FONTSIZE)
        self.prev_dir_rect = pg.Rect(0,TITLE_HEIGHT,TRIANGLE_WIDTH,TRIANGLE_HEIGHT)
        self.next_dir_rect = pg.Rect(self.rect.width-TRIANGLE_WIDTH,TITLE_HEIGHT,TRIANGLE_WIDTH,TRIANGLE_HEIGHT)
        self.prev_file_rect = pg.Rect(0,self.prev_dir_rect.bottom+TRIANGLES_MARGIN,TRIANGLE_WIDTH,TRIANGLE_HEIGHT)
        self.next_file_rect = pg.Rect(self.rect.width-TRIANGLE_WIDTH,self.prev_dir_rect.bottom+TRIANGLES_MARGIN,TRIANGLE_WIDTH,TRIANGLE_HEIGHT)
        self.right_triangle_image, self.left_triangle_image = self.generate_triangle_images()
        self.bg_image = self.generate_bg_image()

    def strip_dirname(self, name):
        base_dir = 'robot_data'
        start = name.index(base_dir)
        end = start + len(base_dir)
        return name[end + 1:]
    
    def strip_filename(self, name):
        base_dir = 'robot_data'
        start = name.index(base_dir)
        end = start + len(base_dir)
        return name[end + 19:]

    def handle_click(self, pos):
        if (pos[0] < self.rect.left or pos[1] > self.rect.bottom):
            return False

        x = pos[0] - self.rect.left
        y = pos[1]

        if util.pos_inside_rect((x,y), self.prev_dir_rect):
            self.prev_dir()
            return True
        if util.pos_inside_rect((x,y), self.next_dir_rect):
            self.next_dir()
        if util.pos_inside_rect((x,y), self.next_file_rect):
            self.next_file()
            return True
        if util.pos_inside_rect((x,y), self.prev_file_rect):
            self.prev_file()
            return True
            return True
    
    def prev_dir(self):
        self.app.telemetry_client.playback_started = False
        self.app.telemetry_client.playback_dir_num = max(self.app.telemetry_client.playback_dir_num - 1, 0)
        self.app.telemetry_client.regenerate_playback_filenames()
        self.app.telemetry_client.playback_file_num = 0
    
    def next_dir(self):
        self.app.telemetry_client.playback_started = False
        self.app.telemetry_client.playback_dir_num = min(len(self.app.telemetry_client.playback_dirnames)-1, self.app.telemetry_client.playback_dir_num+1)
        self.app.telemetry_client.regenerate_playback_filenames()
        self.app.telemetry_client.playback_file_num = 0
    
    def next_file(self):
        self.app.telemetry_client.playback_started = False
        self.app.telemetry_client.playback_file_num = min(self.app.telemetry_client.playback_file_num+1, len(self.app.telemetry_client.playback_filenames)-1)
    def prev_file(self):
        self.app.telemetry_client.playback_started = False
        self.app.telemetry_client.playback_file_num = max(self.app.telemetry_client.playback_file_num-1, 0)

    def generate_triangle_images(self):
        image = pg.Surface((TRIANGLE_WIDTH, TRIANGLE_HEIGHT))
        image.fill(BG_GREY)
        pg.draw.polygon(image, TRIANGLE_COLOUR, ((0,0),(TRIANGLE_WIDTH,TRIANGLE_HEIGHT/2),(0,TRIANGLE_HEIGHT)))
        return image, pg.transform.flip(image, True, False)

    def generate_dirname_image(self):
        image = pg.Surface(self.dirname_rect.size)
        image.fill(BG_GREY)
        dirname_surf = self.dirname_font.render(self.strip_dirname(self.app.telemetry_client.playback_dir), True, FONT_COLOUR)
        image.blit(dirname_surf, (0,1))
        return image
    
    def generate_filename_image(self):
        image = pg.Surface(self.filename_rect.size)
        image.fill(BG_GREY)
        filename_surf = self.dirname_font.render(self.strip_filename(self.app.telemetry_client.playback_filenames[self.app.telemetry_client.playback_file_num]), True, FONT_COLOUR)
        image.blit(filename_surf, (0,1))
        return image

    def generate_bg_image(self):
        image = pg.Surface(self.rect.size)
        image.fill(BG_GREY)

        title_surf = self.title_font.render('Playback', True, FONT_COLOUR)
        image.blit(title_surf, (10, 10))

        image.blit(self.left_triangle_image, self.prev_dir_rect)
        image.blit(self.left_triangle_image, self.prev_file_rect)
        image.blit(self.right_triangle_image, self.next_dir_rect)
        image.blit(self.right_triangle_image, self.next_file_rect)
        return image

    def render_init(self):
        self.app.screen.blit(self.bg_image,self.rect)
    
    def render(self):
        if self.app.telemetry_client.playback_file_num is None or not self.app.telemetry_client.playback_filenames:
            print('One of these == None:')
            print(f'self.app.telemetry_client.playback_file_num: {self.app.telemetry_client.playback_file_num}')
            print(f'self.app.telemetry_client.playback_filenames: {self.app.telemetry_client.get_playback_filenames()}')
            time.sleep(0.3)
            return
        #  print(f'self.app.telemetry_client.playback_filenames[self.app.telemetry_client.playback_file_num]: {self.app.telemetry_client.playback_filenames[self.app.telemetry_client.playback_file_num]}')
        if self.last_dirname != self.app.telemetry_client.playback_dirnames[self.app.telemetry_client.playback_dir_num]:
            #  time.sleep(0.3)
            #  print('regenerating dirname thingy, should be:')
            #  print(self.app.telemetry_client.playback_dir_num)
            #  print(self.app.telemetry_client.playback_dirnames[self.app.telemetry_client.playback_dir_num])
            self.app.screen.blit(self.generate_dirname_image(), (self.rect.left+self.dirname_rect.left, self.rect.top + TITLE_HEIGHT + DIRNAME_TOP_MARGIN))
            self.last_dirname = self.app.telemetry_client.playback_dirnames[self.app.telemetry_client.playback_dir_num]
        #  print(f'self.app.telemetry_client.playback_file_num: {self.app.telemetry_client.playback_file_num}')
        #  print(f'self.app.telemetry_client.playback_filenames: {self.app.telemetry_client.playback_filenames}')
        if self.last_filename != self.app.telemetry_client.playback_filenames[self.app.telemetry_client.playback_file_num]:
            self.app.screen.blit(self.generate_filename_image(), (self.rect.left+self.filename_rect.left, self.rect.top + TITLE_HEIGHT + FILENAME_TOP_MARGIN*2 + FILENAME_HEIGHT))
            self.last_filename = self.app.telemetry_client.playback_filenames[self.app.telemetry_client.playback_file_num]
