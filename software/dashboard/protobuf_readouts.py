import pygame as pg

from constants import *

READOUT_HEIGHT = SCREEN_SIZE[1]-ARENA_SIZE_PIXELS-CONTROL_ITEM_HEIGHT-3*GLOBAL_MARGIN
READOUT_WIDTH = CONTROL_ITEM_WIDTH
READOUT_H_MARGIN = GLOBAL_MARGIN
READOUT_T_MARGIN = GLOBAL_MARGIN
TITLE_HEIGHT = 24
TITLE_L_MARGIN = 8
ITEM_R_PAD = 5
ITEM_T_MARGIN = 2
ITEM_VALUE_RECT_WIDTH = 40
ITEM_VALUE_H_MARGIN = 5
ITEM_VALUE_L_PAD = 5
ITEM_HEIGHT = 14
ITEM_VALUE_RECT_HEIGHT = ITEM_HEIGHT
ITEM_VALUE_BG_COLOUR = (140,140,140)
ITEM_VALUE_FONT_COLOUR = (0,0,0)
ITEM_LABEL_FONT_COLOUR = (255,255,255)
READOUT_BG_COLOUR = (70,70,70)
CONTROL_LABEL_FONT_COLOUR = (70,70,70)
ITEM_LABEL_FONT_SIZE = 13
ITEM_VALUE_FONT_SIZE = 12
TITLE_FONT_SIZE = 20

class ReadoutItem:
    def __init__(self, name, proto, is_msg):
        self.rect = pg.Rect(0,0,READOUT_WIDTH, ITEM_HEIGHT)
        self.value_rect = pg.Rect(self.rect.width - ITEM_VALUE_H_MARGIN - ITEM_VALUE_RECT_WIDTH,
                                  0,
                                  ITEM_VALUE_RECT_WIDTH,
                                  ITEM_VALUE_RECT_HEIGHT)
        self.is_msg = is_msg
        self.label_font = pg.font.SysFont('Arial', ITEM_LABEL_FONT_SIZE)
        self.value_font = pg.font.SysFont('Arial', ITEM_VALUE_FONT_SIZE)
        self.name = name
        self.proto = proto
        self.bg_image = self.generate_bg_image()
        self.value_image = self.generate_value_image()
        self.value_text_image = self.generate_value_text_image()
        self.name = name

    def generate_bg_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill((0,0,0,0))
        label_surf = self.label_font.render(self.name, True, ITEM_LABEL_FONT_COLOUR)
        label_rect = label_surf.get_rect()
        label_top_offset = (self.rect.height - label_rect.height)/2
        if self.is_msg:
            label_rect.left = self.rect.left + ITEM_VALUE_H_MARGIN
        else:
            label_rect.right = self.rect.width - ITEM_VALUE_RECT_WIDTH - 2 * ITEM_VALUE_H_MARGIN
        image.blit(label_surf,label_rect)
        return image

    def generate_value_image(self):
        image = pg.Surface(self.value_rect.size).convert_alpha()
        if self.is_msg:
            image.fill((0,0,0,0))
        else:
            image.fill(ITEM_VALUE_BG_COLOUR)
        return image
    
    def generate_value_text_image(self):
        if self.is_msg:
            return self.value_image # blank
        image = pg.Surface(self.value_rect.size).convert_alpha()
        image.fill((0,0,0,0))
        # TODO handle different types here so text fits nice eg. .02f
        value_surf = self.value_font.render(f'{self.value}', True, ITEM_VALUE_FONT_COLOUR)
        value_height = value_surf.get_rect().height
        value_top_offset = (ITEM_VALUE_RECT_HEIGHT - value_height)/2
        image.blit(value_surf,(ITEM_VALUE_L_PAD,value_top_offset))
        return image

    def update_value_text_image(self):
        self.value_text_image = self.generate_value_text_image()

    @property
    def value(self):
        return getattr(self.proto, self.name)

class Readout:
    def __init__(self, proto, submsgs=[]):
        self.rect = pg.Rect(0,0,READOUT_WIDTH,READOUT_HEIGHT)
        self.n = None
        self.proto = proto
        self.title = type(self.proto).__name__
        self.title_font = pg.font.SysFont('Arial', TITLE_FONT_SIZE)

        fields = self.proto.DESCRIPTOR.fields_by_name.keys()
        data_pairs = dict([(name,getattr(proto, name)) for name in fields])
        self.items = []
        for k, v in data_pairs.items():
            if len(str(type(v))) > 13 and str(type(v))[8:13] == 'proto':
                self.items.append(ReadoutItem(name=k, proto=v, is_msg=True))
                subfields = v.DESCRIPTOR.fields_by_name.keys()
                sub_data_pairs = dict([(name,getattr(v, name)) for name in subfields])
                for sub_k, sub_v in sub_data_pairs.items():
                    self.items.append(ReadoutItem(name=sub_k, proto=v, is_msg=False))
            else:
                self.items.append(ReadoutItem(name=k, proto=proto, is_msg=False))
        
    def position(self, col, row, n):
        self.col = col
        self.row = row
        self.rect.height = self.rect.height/n-(n-1)*READOUT_T_MARGIN
        self.rect.top = ARENA_SIZE_PIXELS+READOUT_T_MARGIN+(self.rect.height+READOUT_T_MARGIN)*row
        self.rect.left = READOUT_H_MARGIN+col*(self.rect.width+READOUT_H_MARGIN)
        self.bg_image = self.generate_bg_image()

    def generate_bg_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill(READOUT_BG_COLOUR)
        title_surf = self.title_font.render(self.title, True, ITEM_LABEL_FONT_COLOUR)
        title_height = title_surf.get_rect().height
        title_top_offset = (TITLE_HEIGHT - title_height)/2
        image.blit(title_surf,(TITLE_L_MARGIN, title_top_offset))
        v_offset = TITLE_HEIGHT
        for item in self.items:
            image.blit(item.bg_image, (0, v_offset))
            image.blit(item.value_image, (item.value_rect.left, v_offset))
            v_offset += ITEM_HEIGHT+ITEM_T_MARGIN
        return image

    def render_init(self):
        self.screen.blit(self.bg_image, self.rect)
        v_offset = TITLE_HEIGHT
        for item in self.items:
            self.screen.blit(item.value_text_image, (self.rect.left+item.value_rect.left, self.rect.top+v_offset))
            v_offset += ITEM_HEIGHT + ITEM_T_MARGIN

    def render(self):
        v_offset = TITLE_HEIGHT
        for item in self.items:
            item.update_value_text_image()
            self.screen.blit(item.value_image, (self.rect.left+item.value_rect.left, self.rect.top+v_offset))
            self.screen.blit(item.value_text_image, (self.rect.left+item.value_rect.left, self.rect.top+v_offset))
            v_offset += ITEM_HEIGHT


class ReadoutGroup:
    def __init__(self, readouts, app):
        self.readouts = readouts
        self.col = None
        self.app = app
        self.screen = self.app.screen
        for readout in self.readouts:
            readout.app = app
            readout.screen = app.screen

    def position(self, col):
        self.col = col
        for row, readout in enumerate(self.readouts):
            readout.position(col, row, len(self.readouts))
    
    def render_init(self):
        for r in self.readouts:
            r.render_init()

    def render(self):
        for r in self.readouts:
            r.render()
        


class ProtobufReadouts:
    def __init__(self, app):
        self.app = app
        self.screen = self.app.screen
        self.cmd_readout = Readout(self.app.cmd_data)
        self.nav_readout = Readout(self.app.nav_data)
        self.guidance_readout = Readout(self.app.guidance_data)
        self.hms_readout = Readout(self.app.hms_data)
        self.imu_readout = Readout(self.app.imu_data)
        self.tof_readouts = [Readout(self.app.tof_data[i]) for i in range(4)]

        readout_columns = [[self.cmd_readout],
                           [self.nav_readout],
                           [self.guidance_readout],
                           [self.hms_readout],
                           [self.imu_readout],
                           self.tof_readouts] # CHANGE THIS LINE!

        self.readout_groups = [ReadoutGroup(i, self.app) for i in readout_columns]
        #  self.comms_toggle = Toggle("comms", self.app.comms_enabled, 2, self.screen, callback=self.app.toggle_comms)
        for col,i in enumerate(self.readout_groups):
            i.position(col)
            i.render_init()

    #  def handle_click(self, pos):
        #  for toggle in self.toggles:
            #  rect = toggle.toggle_rect_globally_positioned
            #  if pos_inside_rect(pos, rect):
                #  toggle.toggle()

    #  def render(self):
        #  for toggle in self.toggles:
            #  toggle.render()
