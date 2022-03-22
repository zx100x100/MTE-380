import pygame as pg

from util import pos_inside_rect

from constants import *
SIM_DEFAULT = False

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

BUTTON_DISABLED_BG_COLOUR = (70,70,70)
START_BUTTON_BG_COLOUR = (0,200,0)
STOP_BUTTON_BG_COLOUR = (200,0,0)

STOP_BUTTON_BG_COLOUR = (200,0,0)
CONNECT_BUTTON_BG_COLOUR = (100,100,255)
DISCONNECT_BUTTON_BG_COLOUR = (150,0,0)

class Toggle:
    def __init__(self, title, on, callback=None):
        self.on = on
        self.title = title
        self.callback = callback # for special behavior when clicked

    def position(self, app, n):
        self.app = app
        self.screen = self.app.screen
        self.rect = pg.Rect(0,0,CONTROL_WIDTH, CONTROL_HEIGHT)
        self.rect.bottom = SCREEN_SIZE[1]-CONTROL_Y_MARGIN
        #  total_width 
        self.rect.left = CONTROL_X_MARGIN+(CONTROL_WIDTH+CONTROL_X_MARGIN)*n

        # locally positioned WRT outer rect for convenience when rendering
        self.toggle_rect = pg.Rect(0,0,TOGGLE_WIDTH,TOGGLE_HEIGHT)
        self.toggle_rect.right = self.rect.width - CONTROL_X_PAD
        self.toggle_rect.centery = self.rect.height/2

        # globally positioned version of the above for convenient click detection
        self.click_rect_globally_positioned = pg.Rect(0,0,TOGGLE_WIDTH,TOGGLE_HEIGHT)
        self.click_rect_globally_positioned.left = self.rect.left + self.toggle_rect.left
        self.click_rect_globally_positioned.centery = self.rect.centery

        self.slider_rect = pg.Rect(0,0,TOGGLE_SLIDER_WIDTH, TOGGLE_SLIDER_HEIGHT)
        self.slider_rect.centery = self.rect.height/2
        self.title_font = pg.font.SysFont('Arial', CONTROL_LABEL_FONT_SIZE)
        self.horizontally_adjust_rect()
        self.image = self.generate_image()


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

    def click(self):
        self.on = not self.on
        self.horizontally_adjust_rect()
        self.image = self.generate_image()
        self.render()
        if self.callback is not None:
            self.callback()

class Button:
    def __init__(self, text_callback, click_callback, colour_callback=lambda: CONTROL_BG_COLOUR, is_disabled_callback=lambda: False):
        self.text_callback = text_callback # for special behavior when clicked
        self.click_callback = click_callback # for special behavior when clicked
        self.is_disabled_callback = is_disabled_callback # for when button should be disabled
        self.colour_callback = colour_callback # for when button should be disabled

    @property
    def disabled(self):
        return self.is_disabled_callback()
    
    @property
    def bg_colour(self):
        return self.colour_callback()
    
    @property
    def disabled(self):
        return self.is_disabled_callback()

    @property
    def title(self):
        return self.text_callback()

    def position(self, app, n):
        self.n = n
        self.app = app
        self.screen = app.screen
        self.rect = pg.Rect(0,0,CONTROL_WIDTH, CONTROL_HEIGHT)
        self.rect.bottom = SCREEN_SIZE[1]-CONTROL_Y_MARGIN
        self.rect.left = CONTROL_X_MARGIN+(CONTROL_WIDTH+CONTROL_X_MARGIN)*n
        self.click_rect_globally_positioned = self.rect

        self.title_font = pg.font.SysFont('Arial', CONTROL_LABEL_FONT_SIZE)
        self.image = self.generate_image()

    def refresh_state(self):
        self.image = self.generate_image()
        self.render()

    def generate_image(self):
        image = pg.Surface(self.rect.size).convert_alpha()
        if self.disabled:
            image.fill(BUTTON_DISABLED_BG_COLOUR)
        else:
            image.fill(self.bg_colour)
        title_surf = self.title_font.render(self.title, True, CONTROL_LABEL_FONT_COLOUR)
        title_height = title_surf.get_rect().height
        title_top_offset = (CONTROL_HEIGHT - title_height)/2
        image.blit(title_surf,(5,title_top_offset))
        
        return image
    

    def render(self):
        self.screen.blit(self.image, self.rect)

    def click(self):
        if not self.disabled:
            self.click_callback()
            self.image = self.generate_image()
            self.render()
        else:
            pass

class Controls:
    def __init__(self, app):
        self.app = app
        self.screen = self.app.screen
        self.teleop = True
        self.sim = SIM_DEFAULT
        self.telemetry_enabled = True

        self.connect_button = Button(text_callback=self.connect_button_text,
                                click_callback=self.connect_button_click,
                                colour_callback=self.connect_button_colour)
        self.start_button = Button(text_callback=self.start_button_text,
                              click_callback=self.start_button_click,
                              colour_callback=self.start_button_colour,
                              is_disabled_callback=self.start_button_disabled)
        self.teleop_toggle = Toggle("teleop", self.teleop, callback=self.toggle_teleop)
        self.sim_toggle = Toggle("sim", self.sim, callback=self.toggle_sim)
        self.telemetry_toggle = Toggle("telemetry", self.telemetry_enabled, callback=self.toggle_telemetry)
        self.elements = [self.connect_button, self.start_button, self.teleop_toggle, self.sim_toggle, self.telemetry_toggle]
        self.position_elements()
    
    def toggle_telemetry(self):
        self.telemetry_enabled = not self.telemetry_enabled
        self.app.data.cmd.pb.disableTelemetry = not self.telemetry_enabled

    def toggle_teleop(self):
        self.teleop = not self.teleop
        self.set_run_state()
    
    def toggle_sim(self):
        self.sim = not self.sim
        self.set_run_state()

    def connect_button_click(self):
        if self.app.telemetry_client.connected:
            self.app.telemetry_client.disconnectme = True
            # refresh_state gets called by main soon anyway
            #  self.start_button.refresh_state()
        else:
            self.app.telemetry_client.connect()
            self.start_button.refresh_state()
    
    def connect_button_text(self):
        return 'Disconnect' if self.app.telemetry_client.connected else 'Connect'
    
    def connect_button_colour(self):
        if self.app.telemetry_client.connected:
            return DISCONNECT_BUTTON_BG_COLOUR
        else:
            return CONNECT_BUTTON_BG_COLOUR

    def robot_is_started(self):
        return self.app.data.cmd.pb.runState in STARTED_STATES
    
    def start_button_text(self):
        if self.robot_is_started():
            return 'Stop'
        else:
            return 'Start'
    
    def start_button_colour(self):
        if self.robot_is_started():
            return STOP_BUTTON_BG_COLOUR
        else:
            return START_BUTTON_BG_COLOUR
    
    def start_button_click(self):
        self.inverse_run_state()
    
    def set_run_state(self):
        if self.robot_is_started():
            # then we should stop the robot
            if self.sim:
                self.app.data.cmd.pb.runState = CmdData.RunState.SIM
            elif self.teleop:
                self.app.data.cmd.pb.runState = CmdData.RunState.TELEOP
            else:
                self.app.data.cmd.pb.runState = CmdData.RunState.AUTO
        else:
            self.app.data.cmd.pb.runState = CmdData.RunState.E_STOP

    def inverse_run_state(self):
        if self.robot_is_started():
            # then we should stop the robot
            self.app.data.cmd.pb.runState = CmdData.RunState.E_STOP
        else:
            if self.sim:
                self.app.data.cmd.pb.runState = CmdData.RunState.SIM
            elif self.teleop:
                self.app.data.cmd.pb.runState = CmdData.RunState.TELEOP
            else:
                self.app.data.cmd.pb.runState = CmdData.RunState.AUTO

    
    def start_button_disabled(self):
        #  return False
        if self.app.data.cmd.pb.runState is CmdData.RunState.SIM or self.sim:
            return False
        return not self.app.telemetry_client.connected

    def position_elements(self):
        for n,element in enumerate(self.elements):
            element.position(self.app, n)

    def handle_click(self, pos):
        for element in self.elements:
            rect = element.click_rect_globally_positioned
            if pos_inside_rect(pos, rect):
                element.click()
                return True

    def render_init(self):
        for toggle in self.elements:
            toggle.render()

