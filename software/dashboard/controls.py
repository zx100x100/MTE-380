import pygame as pg

from util import pos_inside_rect

from constants import *
CONTROL_WIDTH = CONTROL_ITEM_WIDTH
CONTROL_HEIGHT = CONTROL_ITEM_HEIGHT
TOGGLE_WIDTH = 60
CONTROL_X_PAD = 4
CONTROL_Y_PAD = 2
TOGGLE_HEIGHT = CONTROL_HEIGHT - CONTROL_Y_PAD*2
TOGGLE_X_PAD = 4
TOGGLE_Y_PAD = 2
TOGGLE_SLIDER_HEIGHT = TOGGLE_HEIGHT - TOGGLE_Y_PAD*2
TOGGLE_SLIDER_WIDTH = TOGGLE_SLIDER_HEIGHT
TOGGLE_WIDTH = TOGGLE_SLIDER_WIDTH*2
#  TOGGLE_SLIDER_X_PAD = 5
TOGGLE_SLIDER_BG_COLOUR = (200,200,200)
TOGGLE_ON_BG_COLOUR = (0,130,0)
TOGGLE_OFF_BG_COLOUR = (100,15,15)
CONTROL_BG_COLOUR = (100,100,120)
CONTROL_LABEL_FONT_COLOUR = (0,0,0)
CONTROL_X_MARGIN = GLOBAL_MARGIN
CONTROL_Y_MARGIN = GLOBAL_MARGIN

CONTROL_LABEL_FONT_SIZE = 18

class Toggle:
    def __init__(self, title, on, n, screen, callback=None):
        self.on = on
        self.title = title
        self.screen = screen
        self.n = n
        self.rect = pg.Rect(0,0,CONTROL_WIDTH, CONTROL_HEIGHT)
        self.rect.bottom = SCREEN_SIZE[1]-CONTROL_Y_MARGIN
        #  total_width 
        self.rect.left = CONTROL_X_MARGIN+(CONTROL_WIDTH+CONTROL_X_MARGIN)*n

        # locally positioned WRT outer rect for convenience when rendering
        self.toggle_rect = pg.Rect(0,0,TOGGLE_WIDTH,TOGGLE_HEIGHT)
        self.toggle_rect.right = self.rect.width - CONTROL_X_PAD
        self.toggle_rect.centery = self.rect.height/2

        # globally positioned version of the above for convenient click detection
        self.toggle_rect_globally_positioned = pg.Rect(0,0,TOGGLE_WIDTH,TOGGLE_HEIGHT)
        self.toggle_rect_globally_positioned.left = self.rect.left + self.toggle_rect.left
        self.toggle_rect_globally_positioned.centery = self.rect.centery

        self.slider_rect = pg.Rect(0,0,TOGGLE_SLIDER_WIDTH, TOGGLE_SLIDER_HEIGHT)
        self.slider_rect.centery = self.rect.height/2
        self.title_font = pg.font.SysFont('Arial', CONTROL_LABEL_FONT_SIZE)
        self.horizontally_adjust_rect()
        self.image = self.generate_image()

        self.callback = callback # for special behavior when clicked

    def horizontally_adjust_rect(self):
        if self.on:
            self.slider_rect.right = self.toggle_rect.right - TOGGLE_X_PAD
        else:
            self.slider_rect.left = self.toggle_rect.left + TOGGLE_X_PAD


    def generate_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        image.fill(CONTROL_BG_COLOUR)
        title_surf = self.title_font.render(self.title, True, CONTROL_LABEL_FONT_COLOUR)
        title_height = title_surf.get_rect().height
        title_top_offset = (CONTROL_HEIGHT - title_height)/2
        image.blit(title_surf,(5,title_top_offset))

        toggle_bg_image = pg.Surface(self.toggle_rect.size).convert_alpha()
        toggle_bg_image.fill(TOGGLE_ON_BG_COLOUR if self.on else TOGGLE_OFF_BG_COLOUR)
        image.blit(toggle_bg_image, self.toggle_rect.topleft)

        toggle_slider_image = pg.Surface(self.slider_rect.size).convert_alpha()
        toggle_slider_image.fill(TOGGLE_SLIDER_BG_COLOUR)
        image.blit(toggle_slider_image, self.slider_rect.topleft)
        
        return image
    

    def render(self):
        self.screen.blit(self.image, self.rect)

    def toggle(self, force_enable=None):
        self.on = not self.on
        if force_enable is not None:
            self.on = force_enable
        self.horizontally_adjust_rect()
        self.image = self.generate_image()
        self.render()
        if self.callback is not None:
            self.callback()

class Controls:
    def __init__(self, app):
        self.app = app
        self.screen = self.app.screen
        self.enable_toggle = Toggle("enable", False, 0, self.screen, callback=self.app.toggle_robot_enable)
        self.propeller_toggle = Toggle("propeller", False, 1, self.screen)
        self.teleop_toggle = Toggle("teleop", True, 2, self.screen, callback=self.app.toggle_teleop)
        self.comms_toggle = Toggle("comms", self.app.comms_enabled, 3, self.screen, callback=self.app.toggle_comms)
        self.toggles = [self.enable_toggle, self.propeller_toggle, self.teleop_toggle, self.comms_toggle]

    def handle_click(self, pos):
        for toggle in self.toggles:
            rect = toggle.toggle_rect_globally_positioned
            if pos_inside_rect(pos, rect):
                toggle.toggle()
                return True

    def render(self):
        for toggle in self.toggles:
            toggle.render()

