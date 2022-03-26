import pygame as pg
from collections import deque
from itertools import islice
import math
import traceback
import time

from constants import *

Y_LABEL_WIDTH = 80
PLOT_BACKGROUND_COLOUR = (255,255,255) # where the actual data goes (inner)
DATA_POINT_COLOUR = (200,0,200) # where the actual data goes (inner)
Y_LABEL_BACKGROUND_COLOUR = (0,200,60)
TITLE_RECT_HEIGHT = 30
PAD = (TITLE_RECT_HEIGHT, 0, 0, Y_LABEL_WIDTH)
BACKGROUND_COLOUR = (100,100,120) # only shows up for TITLE!
TITLE_COLOUR = (0,0,0)
AXIS_LABEL_COLOUR = (25,25,25)
AXIS_LABEL_FONTSIZE = 12
DATA_POINT_SIZE = 1 # pixels of diameter/width per data point square/circle
DATA_POINT_SIZE = 1
TICK_SIZE = (Y_LABEL_WIDTH,15)
DISPLAY_MIN_N_TICKS = 3
DISPLAY_MAX_N_TICKS = 5
MIN_TICKS_BETW_RESCALES = 300 # prevent twitchy rescales
MAX_S_BETWEEN_RESCALES = 0.5

PLOT_MARGIN = (GLOBAL_MARGIN, 0, 0, GLOBAL_MARGIN)

class TelemetryPlot:
    def __init__(self, pb_item, row, col, tick_increment=3.0):
        #  self.proto = proto
        self.pb_item = pb_item
        self.title = self.pb_item.name
        self.tick_increment = tick_increment
        self.plot_size = PLOT_SIZE
        self.row = row
        self.col = col
        total_width = PAD[TRBL.L.value]+self.plot_size[0]+PAD[TRBL.R.value]
        total_height = PAD[TRBL.T.value]+self.plot_size[1]+PAD[TRBL.B.value]
        self.rect = pg.Rect(ARENA_SIZE_PIXELS+PLOT_MARGIN[TRBL.L.value] + col*(PLOT_MARGIN[TRBL.L.value] + total_width + PLOT_MARGIN[TRBL.R.value]),
                            PLOT_MARGIN[TRBL.T.value]+row*(PLOT_MARGIN[TRBL.T.value] + total_height + PLOT_MARGIN[TRBL.B.value]),
                     total_width,
                     total_height)
        #  print(f'self.rect: {self.rect}')
        #  print(f'total_width: {total_width}')
        self.title_font = pg.font.SysFont('Arial', 20)
        self.axis_label_font = pg.font.SysFont('Arial', AXIS_LABEL_FONTSIZE)

        # TODO: make self.values an @attribute that grabs DISPLAY_DATA_POINTS
        #  self.values = deque([proto.value],maxlen=DISPLAY_DATA_POINTS)

        self.values = self.pb_item.values
        self.plot_image = self.generate_plot_image()
        self.plot_image_rect = self.plot_image.get_rect()
        self.plot_image_rect.bottom = self.rect.bottom
        self.plot_image_rect.right = self.rect.right
        self.background_image = self.generate_background_image()
        self.y_label_area_image = self.generate_y_label_area_image()
        self.y_label_area_image_rect = self.y_label_area_image.get_rect()
        self.y_label_area_image_rect.left = self.rect.left
        self.y_label_area_image_rect.bottom = self.rect.bottom
        self.scale_factor = 1
        self.display_scale_min = min(self.values)
        self.display_scale_max = max(self.values)
        self.display_scale_pad = 0.3 # 30% - how much to display beyond data
        self.display_tick_pad = 0.15 # 10% - how far to place ticks beyond data
        self.pixel_scale_factor = None
        self.new_value = None
        self.ticks = {}
        self.ticks_since_last_rescale = 0 # prevent twitchy rescaling
        self.last_rescale_t = time.time()

        new_min = min(self.values)
        new_max = max(self.values)
        if new_max == new_min:
            new_min = min(self.values)
            new_max = new_min+0.01
        breadth = new_max-new_min
        new_min_for_display = new_min - breadth*self.display_scale_pad
        new_max_for_display = new_max + breadth*self.display_scale_pad
        new_fullscale = new_max_for_display-new_min_for_display
        new_pixel_scale_factor = self.plot_size[1]/new_fullscale
        self.pixel_scale_factor = new_pixel_scale_factor

    # return MAX_DATA_POINTS items from the end of the values deque
    # except move the entire slice left by one since we use this to erase
    # the previous state
    #  @property
    #  def old_values(self):
        #  length = len(self.pb_item.values)
        #  first_ele = max(length - DISPLAY_DATA_POINTS - 1, 0)
        #  #  print(f'first_ele: {first_ele}')
        #  return deque(islice(self.pb_item.values, first_ele, len(self.pb_item.values)-1))

    # return MAX_DATA_POINTS items from the end of the values deque
    #  @property
    #  def values(self):
        #  return self.pb_item.values
        #  length = len(self.pb_item.values)
        #  first_ele = length - DISPLAY_DATA_POINTS
        #  if first_ele<1:
            #  return self.pb_item.values
        #  return deque(islice(self.pb_item.values, first_ele, len(self.pb_item.values)))

    def render_init(self, screen):
        screen.blit(self.background_image, self.rect)
        screen.blit(self.plot_image, self.plot_image_rect)
        screen.blit(self.y_label_area_image, self.y_label_area_image_rect)
    
    def erase(self, screen):
        image = pg.Surface(self.rect.size)
        image.fill((0,0,0))
        screen.blit(image, self.rect)

    def erase_update_render(self, screen):
        #  print('eur.',end='')
        self.ticks_since_last_rescale += 1
        if self.pb_item.plotted_latest_value:
            #  pass
            return
        else:
            self.pb_item.plotted_latest_value = True
        #  print('start erase_update_render')
        try:
            #  print(f'self.values: {self.values}')
            screen.blit(self.plot_image, self.plot_image_rect)
            #  if self.pixel_scale_factor is not None:
                #  for i in range(len(self.old_values)):
                    #  #  pass # erase broken rn
                    #  breadth = self.display_scale_max-self.display_scale_min
                    #  minn = self.display_scale_min-breadth*self.display_scale_pad
                    #  #  print(f'self.old_values: {self.old_values}')
                    #  #  print(len(self.old_values))
                    #  #  print(f'i: {i}')
                    #  dist_from_bot = self.old_values[-(i+1)]-minn
                    #  scaled_dist_from_bot = int(dist_from_bot*self.pixel_scale_factor)
                    #  height = self.rect.bottom-scaled_dist_from_bot
                    #  pixel_pos = (self.rect.right-i-1,height)
                    
                    #  #  print(f'erase_pixel_pos: {pixel_pos}')
                    #  screen.set_at(pixel_pos, PLOT_BACKGROUND_COLOUR)
                    #  #  screen.set_at(pixel_pos, (0,255,0))
            
            new_min = min(self.values)
            new_max = max(self.values)
            if new_max == new_min:
                new_min -= 0.01
                new_max += 0.01
                iterate_ticks = False
            else:
                iterate_ticks = True
            breadth = new_max-new_min

            #  print(f'breadth: {breadth}')

            new_min_for_display = new_min - breadth*self.display_scale_pad
            #  print(f'new_min_for_display: {new_min_for_display}')
            new_max_for_display = new_max + breadth*self.display_scale_pad
            #  print(f'new_max_for_display: {new_max_for_display}')
            new_min_for_ticks = new_min - breadth*self.display_tick_pad
            new_max_for_ticks = new_max + breadth*self.display_tick_pad

            if new_min != self.display_scale_min or new_max != self.display_scale_max:
                new_fullscale = new_max_for_display-new_min_for_display
                new_pixel_scale_factor = self.plot_size[1]/new_fullscale

                first_tick = (math.ceil(new_min_for_ticks/self.tick_increment))*self.tick_increment

                last_tick = (math.floor(new_max_for_ticks / self.tick_increment))*self.tick_increment
                new_time = time.time()
                dt = new_time - self.last_rescale_t
                if (self.ticks_since_last_rescale > MIN_TICKS_BETW_RESCALES or dt > MAX_S_BETWEEN_RESCALES) and iterate_ticks:
                    self.last_rescale_t = new_time
                    n=0
                    while (last_tick-first_tick)/self.tick_increment < DISPLAY_MIN_N_TICKS:
                        if n>30:
                            break
                        n += 1
                        first_tick = (math.ceil(new_min_for_ticks/self.tick_increment))*self.tick_increment

                        last_tick = (math.floor(new_max_for_ticks/self.tick_increment))*self.tick_increment
                        self.ticks_since_last_rescale = 0
                        self.tick_increment /= 1.1**n

                    n=0
                    while (last_tick-first_tick)/self.tick_increment > DISPLAY_MAX_N_TICKS:
                        if n>30:
                            break
                        n += 1
                        self.tick_increment *= 1.1**n
                        first_tick = (math.ceil(new_min_for_ticks/self.tick_increment))*self.tick_increment

                        last_tick = (math.floor(new_max_for_ticks/self.tick_increment))*self.tick_increment
                        self.ticks_since_last_rescale = 0

                new_tick = first_tick
                new_ticks = {}
                screen.blit(self.y_label_area_image, self.y_label_area_image_rect)
                tick_breadth = last_tick-first_tick
                n_ticks = tick_breadth/self.tick_increment
                #  if n_ticks > DISPLAY_MAX_N_TICKS:
                #  print(f'skip_by: {skip_by}')
                while(new_tick) <= last_tick:
                    new_tick_str = f'{new_tick:.5f}'
                    #  print(f'new_tick_str: {new_tick_str}')
                    try:
                        new_ticks[new_tick_str] = self.ticks[new_tick]
                    except KeyError:
                        new_ticks[new_tick_str] = self.generate_tick(new_tick)

                    new_tick_offset = (new_tick-new_min_for_display)*new_pixel_scale_factor+TICK_SIZE[1]/2
                    new_tick_height = self.rect.bottom - new_tick_offset
                    #  print(f'new - min: {new_tick-new_min_for_display}')
                    #  print(f'scalefac: {new_pixel_scale_factor}')
                    #  print(f'offset: {new_tick_offset}')
                    #  print(f'height: {new_tick_height}')
                    screen.blit(new_ticks[new_tick_str], (self.rect.left,new_tick_height))
                    new_tick += self.tick_increment

                self.ticks = new_ticks
                self.pixel_scale_factor = new_pixel_scale_factor
            self.display_scale_max = new_max
            self.display_scale_min = new_min

            #  print('out.')
            for i in range(len(self.values)):
                minn = self.display_scale_min-breadth*self.display_scale_pad
                dist_from_bot = self.values[-(i+1)]-minn
                scaled_dist_from_bot = int(dist_from_bot*self.pixel_scale_factor)
                height = self.rect.bottom-scaled_dist_from_bot
                pixel_pos = (self.rect.right-i-1,height)
                #  print(f'self.rect: {self.rect}')
                screen.set_at(pixel_pos,DATA_POINT_COLOUR)
                #  print(f'out_pixel_pos: {pixel_pos}')
        except Exception as e:
            #  pass
            print(f"ERROR UPDATING PLOTS: {e}")
            print(f"{traceback.format_exc()}")
        #  print('end erase_update_render')

# (200,0,200)
        #  self.values.append()
    def generate_tick(self,val):
        image = pg.Surface((TICK_SIZE[0],TICK_SIZE[1])).convert_alpha()
        image.fill(Y_LABEL_BACKGROUND_COLOUR)
        tick_label_surf = self.axis_label_font.render(f'{val:.3f}-', True, TITLE_COLOUR)
        label_rect = tick_label_surf.get_rect()
        image.blit(tick_label_surf, (TICK_SIZE[0]-label_rect.width, TICK_SIZE[1]/2 - label_rect.height/2))
        return image

    def generate_y_label_area_image(self):
        image = pg.Surface((Y_LABEL_WIDTH,self.plot_size[1])).convert_alpha()
        image.fill(Y_LABEL_BACKGROUND_COLOUR)
        return image

    def generate_plot_image(self):
        image = pg.Surface((self.plot_size[0], self.plot_size[1])).convert_alpha()
        image.fill(PLOT_BACKGROUND_COLOUR)
        return image

    def generate_background_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill(BACKGROUND_COLOUR)
        title_surf = self.title_font.render(self.title, True, TITLE_COLOUR)
        title_height = title_surf.get_rect().height
        title_top_offset = (TITLE_RECT_HEIGHT - title_height)/2
        image.blit(title_surf,(5,title_top_offset))
        #  value_surf = self.title_font.render(f'{self.values[-1]:.4f}', False, TITLE_COLOUR)
        #  width_of_val = value_surf.get_rect().width
        #  image.blit(value_surf,(self.rect.width-width_of_val,5))
        return image
