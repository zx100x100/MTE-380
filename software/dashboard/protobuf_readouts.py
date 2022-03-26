import pygame as pg
from collections import deque
import math
import os

from util import (pos_inside_rect, next_enum)
from proto.hms_and_cmd_data_pb2 import HmsData
from constants import *

READOUT_HEIGHT = SCREEN_SIZE[1]-ARENA_SIZE_PIXELS-CONTROL_ITEM_HEIGHT-3*GLOBAL_MARGIN
READOUT_WIDTH = CONTROL_ITEM_WIDTH
if UGLY_SQUEEZE_MODE:
    READOUT_H_MARGIN = 0
else:
    READOUT_H_MARGIN = GLOBAL_MARGIN
READOUT_T_MARGIN = GLOBAL_MARGIN
TITLE_HEIGHT = 24
TITLE_L_MARGIN = 2
ITEM_H_PAD = 5
ITEM_T_MARGIN = 2
ITEM_VALUE_RECT_WIDTH = 80
ITEM_VALUE_H_MARGIN = 5
ITEM_VALUE_L_PAD = 1
ITEM_HEIGHT = 12
ITEM_VALUE_RECT_HEIGHT = ITEM_HEIGHT
ITEM_VALUE_BG_COLOUR = (140,140,140)
ITEM_VALUE_FONT_COLOUR = (0,0,0)
CLICKED_ITEM_VALUE_FONT_COLOUR = (255,0,0)
ITEM_LABEL_FONT_COLOUR = (255,255,255)
READOUT_BG_COLOUR = (70,70,70)
CONTROL_LABEL_FONT_COLOUR = (70,70,70)
ITEM_LABEL_FONT_SIZE = 10
ITEM_VALUE_FONT_SIZE = 10
TITLE_FONT_SIZE = 16
MAX_ITEMS_PER_READOUT = (READOUT_HEIGHT-TITLE_HEIGHT)/(ITEM_HEIGHT+ITEM_T_MARGIN)
ERROR_INFO_FIELD_NAME = "errorInfo"
ERROR_INFO_FONT_SIZE = ITEM_VALUE_FONT_SIZE = 12
ERROR_INFO_LINE_HEIGHT = 14
ERROR_INFO_T_PAD = 5
                
NAV_OFFSET = 0.2
GUIDANCE_OFFSET = 0.1
IMU_OFFSET = 0.25

EPSILON = 0.0000001

class ReadoutItem:
    def __init__(self, name, proto):
        self.rect = pg.Rect(0,0,READOUT_WIDTH, ITEM_HEIGHT)
        self.clicked = False
        self.value_rect = pg.Rect(self.rect.width - ITEM_VALUE_H_MARGIN - ITEM_VALUE_RECT_WIDTH,
                                  0,
                                  ITEM_VALUE_RECT_WIDTH,
                                  ITEM_VALUE_RECT_HEIGHT)
        self.label_font = pg.font.SysFont('Arial', ITEM_LABEL_FONT_SIZE)
        self.value_font = pg.font.SysFont('Arial', ITEM_VALUE_FONT_SIZE)
        self.name = name
        self.proto = proto
        self.is_enum = self.detect_whether_item_is_enum()
        self.is_error_info = self.name == ERROR_INFO_FIELD_NAME

        self.is_numeric = type(self.value) in (int, float)
        self.bg_image = self.generate_bg_image()
        self.value_image = self.generate_value_image()
        self.value_text_image = self.generate_value_text_image()
        self.name = name

        self.values = deque([self.value],maxlen=DISPLAY_DATA_POINTS)
        self.plotted_latest_value = False
    
    def append_value(self, value):
        if self.is_numeric and not self.is_enum:
            if not math.isnan(value):
                self.values.append(value)
                self.plotted_latest_value = False
            else:
                pass
                #  print(f"TRYING TO APPEND ILLEGAL NUMBER: {value}")
        else:
            self.values.append(value)
            self.plotted_latest_value = False


    def update_value(self):
        if self.is_numeric and not self.is_enum:
            if not math.isnan(self.value):
                self.values.append(self.value)
                self.plotted_latest_value = False
            else:
                pass
                #  print(f"TRYING TO APPEND ILLEGAL NUMBER: {self.value}")
        else:
            self.values.append(self.value)
            self.plotted_latest_value = False
    
    def replace_value(self, value):
        self.values[-1] = value
        self.plotted_latest_value = False

    def detect_whether_item_is_enum(self):
        return self.proto.DESCRIPTOR.fields_by_name[self.name].enum_type is not None

    def generate_bg_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill((0,0,0,0))
        label_surf = self.label_font.render(self.name, True, ITEM_LABEL_FONT_COLOUR)
        label_rect = label_surf.get_rect()
        label_top_offset = (self.rect.height - label_rect.height)/2
        if self.is_error_info:
            label_rect.centerx = self.rect.centerx
        else:
            label_rect.right = self.rect.width - ITEM_VALUE_RECT_WIDTH - 2 * ITEM_VALUE_H_MARGIN
        image.blit(label_surf,label_rect)
        return image

    def generate_value_image(self):
        image = pg.Surface(self.value_rect.size).convert_alpha()
        if self.is_error_info:
            image.fill((0,0,0,0))
        else:
            image.fill(ITEM_VALUE_BG_COLOUR)
        return image
    
    def generate_value_text_image(self):
        if self.is_error_info:
            return self.value_image # blank
        image = pg.Surface(self.value_rect.size).convert_alpha()
        image.fill((0,0,0,0))

        # TODO handle different types here so text fits nice eg. .02f
        try:
            if self.is_numeric:
                value_surf = self.value_font.render(f'{self.value:.04f}', True, ITEM_VALUE_FONT_COLOUR if not self.clicked else CLICKED_ITEM_VALUE_FONT_COLOUR)
            else:
                value_surf = self.value_font.render(f'{self.value}', True, ITEM_VALUE_FONT_COLOUR if not self.clicked else CLICKED_ITEM_VALUE_FONT_COLOUR)
        except Exception as e:
            print(f'err rendering value for {self.name}: {e}')
            value_surf = self.value_font.render(f'{self.value}', True, ITEM_VALUE_FONT_COLOUR if not self.clicked else CLICKED_ITEM_VALUE_FONT_COLOUR)
        value_height = value_surf.get_rect().height
        value_top_offset = (ITEM_VALUE_RECT_HEIGHT - value_height)/2+2
        image.blit(value_surf,(ITEM_VALUE_L_PAD,value_top_offset))
        return image

    def update_value_text_image(self):
        self.value_text_image = self.generate_value_text_image()

    def set_clicked(self):
        self.clicked = True

    def set_not_clicked(self):
        self.clicked = False

    @property
    def value(self):
        if self.is_enum:
            enum_type = self.proto.DESCRIPTOR.fields_by_name[self.name].enum_type
            return enum_type.values_by_number[getattr(self.proto, self.name)].name
        if self.is_error_info:
            return self.parse_error_info(getattr(self.proto, self.name))
        #  print(self.proto.DESCRIPTOR.fields_by_name.items())
        return getattr(self.proto, self.name)

    def parse_error_info(self, value):
        if not value:
            value = "1:blah;1:blah2"
        lines = []
        for error_info in value.split(";"):
            sep_ind = error_info.find(":")
            error_code_str, error_desc = error_info[:sep_ind], error_info[sep_ind+1:]
            error_enum_name = HmsData.Error.Name(int(error_code_str))
            #  print(f'error_enum_val: {error_desc}')
            lines.append(f'{error_enum_name}:')
            lines.append(f' {error_desc}')
        return lines


class Readout:
    def __init__(self, proto, app):
        self.n = None
        self.proto = proto
        self.app = app
        self.screen = app.screen
        self.title_font = pg.font.SysFont('Arial', TITLE_FONT_SIZE)
        self.items = self.get_items()
        self.rect, self.double_wide = self.generate_rect()
        self.title = type(self.proto).__name__
        if self.title == 'TofData':
            tof_enum = TofPosition(self.app.tof_readouts_initialized)
            self.title = f'TOF_{tof_enum.value}_{tof_enum.name}'
            self.app.tof_readouts_initialized += 1
        self.toplefts = None # toplets of each item, assigned during positioning

    def update_vals(self):
        for item in self.items:
            item.update_value()
        
    def generate_rect(self):
        if len(self.items)>MAX_ITEMS_PER_READOUT:
            rect = pg.Rect(0,0,READOUT_WIDTH*2+READOUT_H_MARGIN,READOUT_HEIGHT)
            double_wide = True
        else:
            rect = pg.Rect(0,0,READOUT_WIDTH,READOUT_HEIGHT)
            double_wide = False
        return (rect, double_wide)

    def get_items(self):
        fields = self.proto.DESCRIPTOR.fields_by_name.keys()
        data_pairs = dict([(name,getattr(self.proto, name)) for name in fields])
        items = []
        for k, v in data_pairs.items():
            items.append(ReadoutItem(name=k, proto=self.proto))
        return items
        
    def position(self, col, row, n):
        self.col = col
        self.row = row
        self.rect.height = self.rect.height/n

        self.rect.top = ARENA_SIZE_PIXELS+READOUT_T_MARGIN+(self.rect.height+READOUT_T_MARGIN)*row
        self.rect.left = READOUT_H_MARGIN+col*(READOUT_WIDTH+READOUT_H_MARGIN)
        self.toplefts = self.get_topleft_of_each_item()
        self.item_topleft_pairs = list(zip(self.items, self.toplefts))
        self.handle_error_info()

    def handle_error_info(self):
        error_info = [i for i in self.items if i.is_error_info]
        if error_info:
            error_info = error_info[0]
            error_info_topleft = [i[1] for i in self.item_topleft_pairs if i[0] is error_info][0]
            topleft = (error_info_topleft[0]+ITEM_H_PAD, error_info_topleft[1]+ITEM_HEIGHT+ITEM_T_MARGIN)
            height = self.rect.bottom - topleft[1] - READOUT_T_MARGIN / 2
            width = self.rect.width - ITEM_H_PAD*2
            error_info_rect = pg.Rect(topleft[0],topleft[1], width, height)

            error_info_bg = pg.Surface(error_info_rect.size).convert_alpha()
            error_info_bg.fill(ITEM_VALUE_BG_COLOUR)
            self.app.error_info = (error_info, error_info_bg, error_info_rect)

        self.bg_image = self.generate_bg_image()

    def generate_bg_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill(READOUT_BG_COLOUR)
        title_surf = self.title_font.render(self.title, True, ITEM_LABEL_FONT_COLOUR)
        title_height = title_surf.get_rect().height
        title_top_offset = (TITLE_HEIGHT - title_height)/2
        title_h_offset = TITLE_L_MARGIN
        if UGLY_SQUEEZE_MODE:
            if self.title == 'NavData':
                title_h_offset += READOUT_WIDTH * NAV_OFFSET
            elif self.title == 'GuidanceData':
                title_h_offset += READOUT_WIDTH * GUIDANCE_OFFSET
            elif self.title == 'ImuData':
                title_h_offset += READOUT_WIDTH * IMU_OFFSET

        image.blit(title_surf,(title_h_offset, title_top_offset))
        for item, topleft in self.item_topleft_pairs:
            image.blit(item.bg_image, (topleft[0]-self.rect.left,topleft[1]-self.rect.top))
            image.blit(item.value_image, (topleft[0]-self.rect.left+item.value_rect.left, topleft[1]-self.rect.top))
        return image

    def handle_click(self, pos):
        for item, topleft in self.item_topleft_pairs:
            global_rect = pg.Rect(topleft[0],topleft[1],item.rect.width, item.rect.height)
            if pos_inside_rect(pos, global_rect):
                if item.is_enum:
                    # super jank, check if we clicked on a CmdData log level
                    if hasattr(item.proto, 'runState'):
                        enum_val = item.value
                        if enum_val in HmsData.LogLevel.keys():
                            enum_num = getattr(item.proto, item.name)
                            total = len(HmsData.LogLevel.keys())
                            next_enum_num = (enum_num+1) % total
                            enum_type = item.proto.DESCRIPTOR.fields_by_name[item.name].enum_type
                            new_name = enum_type.values_by_number[next_enum_num].name
                            setattr(item.proto, item.name, next_enum_num)
                            item.update_value()
                    return False
                elif item.name in ('kP_vel', 'kP_drift', 'kD_vel', 'kD_drift'):
                    val_str = input(f"input new value for {item.name}({item.value}): ")
                    setattr(item.proto, item.name, float(val_str))
                        #  elif enum_val in CmdData.TelemetryMode.keys():
                            #  enum_num = getattr(item.proto, item.name)
                            #  total = len(CmdData.TelemetryMode.keys())
                            #  next_enum_num = (enum_num+1) % total
                            #  enum_type = item.proto.DESCRIPTOR.fields_by_name[item.name].enum_type
                            #  new_name = enum_type.values_by_number[next_enum_num].name
                            #  setattr(item.proto, item.name, next_enum_num)
                            #  item.update_value()
                    return False

                return item
        return False

    def get_topleft_of_each_item(self):
        toplefts = []
        v_offset = TITLE_HEIGHT
        h_offset = 0
        for n, item in enumerate(self.items):
            h_offset_total = h_offset + self.rect.left#+item.value_rect.left
            v_offset_total = v_offset + self.rect.top
            topleft = (h_offset_total, v_offset_total)
            toplefts.append(topleft)
            if (h_offset == 0) and (n+2>MAX_ITEMS_PER_READOUT):
                if UGLY_SQUEEZE_MODE:
                    h_offset = READOUT_WIDTH+READOUT_H_MARGIN-14
                else:
                    h_offset = READOUT_WIDTH+READOUT_H_MARGIN
                v_offset = TITLE_HEIGHT
            else:
                v_offset += ITEM_HEIGHT + ITEM_T_MARGIN
        return toplefts


    def render_init(self):
        #  print(f'self.rect.left:{self.rect.left}')
        self.screen.blit(self.bg_image, self.rect)

    def render(self):
        for item, topleft in self.item_topleft_pairs:
            item.update_value_text_image()
            self.screen.blit(item.value_image, (topleft[0]+item.value_rect.left,topleft[1]))
            self.screen.blit(item.value_text_image, (topleft[0]+item.value_rect.left,topleft[1]))


class ReadoutGroup:
    def __init__(self, readouts, app):
        self.readouts = readouts
        self.col = None
        self.app = app
        self.screen = self.app.screen

    def position(self, col):
        #  if self.readouts[0].title == 'ImuData':
            #  col += 1
        #  elif self.readouts[0].title.startswith('TOF'):
            #  print('moving tof')
            #  col -= 1

        self.col = col
        for row, r in enumerate(self.readouts):
            r.position(col, row, len(self.readouts))
    
    def render_init(self):
        for r in self.readouts:
            r.render_init()

    def render(self):
        for r in self.readouts:
            r.render()

    def handle_click(self, pos):
        for readout in self.readouts:
            item = readout.handle_click(pos)
            if item:
                return item
        return False


class ProtobufReadouts:
    def __init__(self, app, pb_data_columns):
        self.app = app
        self.screen = self.app.screen
        self.error_info_font = pg.font.SysFont('Arial', ERROR_INFO_FONT_SIZE)

        self.init_readout_groups(pb_data_columns)

    def init_readout_groups(self, pb_data_columns):
        readout_columns = []
        for column in pb_data_columns:
            readout_column = []
            for pb_data in column:
                readout = Readout(pb_data.pb, self.app)
                pb_data.set_readout(readout)
                readout_column.append(readout)
            readout_columns.append(readout_column)

        self.readout_groups = [ReadoutGroup(i, self.app) for i in readout_columns]
        col = 0
        for readout_group in self.readout_groups:
            readout_group.position(col)
            readout_group.render_init()
            if readout_group.readouts[0].double_wide:
                col += 1
            col += 1

            if UGLY_SQUEEZE_MODE:
                print(col)
                if col == 2:
                    col -= NAV_OFFSET
                if math.isclose(col, 4 - NAV_OFFSET, abs_tol=EPSILON):
                    col -= GUIDANCE_OFFSET
                if math.isclose(col, 7 - NAV_OFFSET - GUIDANCE_OFFSET, abs_tol=EPSILON):
                    print('hi')
                    col -= IMU_OFFSET

    def render(self):
        for group in self.readout_groups:
            group.render()

        if self.app.error_info is not None:
            item, bg_image, rect = self.app.error_info
            self.app.screen.blit(bg_image, rect)

            #  print(item.value)
            for n, line in enumerate(item.value):
                line_image = self.error_info_font.render(line, True, ITEM_VALUE_FONT_COLOUR)
                left = rect.left + ITEM_VALUE_L_PAD
                top = rect.top + n * ERROR_INFO_LINE_HEIGHT + ERROR_INFO_T_PAD
                self.app.screen.blit(line_image, (left, top))

            #  words = error_


    def handle_click(self, pos):
        for readout_group in self.readout_groups:
            item = readout_group.handle_click(pos)
            if item:
                return item
        return False
