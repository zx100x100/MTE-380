import pygame as pg
import os
import sys
import random
import numpy as np
import time

from robot import Robot
from arena import Arena
from proto.cmd_data_pb2 import CmdData
from telemetry_plots import TelemetryPlots
from controls import Controls
from data import Data
from telemetry_client import TelemetryClient

from constants import *

# 720 x 720 @ 10 PIXELS_PER_INCH
FPS = 60
SPF = 1/FPS
FAKE = True
FAKE_STATIC = False
FAKE_TELEOP = True
TELEOP_SPEED = 20

CAPTION = "yo momma"

DIRECTION_KEYS = {pg.K_a: (-1, 0),
               pg.K_d: (1, 0),
               pg.K_w: (0,-1),
               pg.K_s: (0, 1)}
ROTATION_KEYS = {'r': pg.K_e, 'l': pg.K_q}
QWAS = {'left_fwd': pg.K_q, 'right_fwd': pg.K_w, 'left_back': pg.K_a, 'right_back': pg.K_s}

class App:
    def __init__(self):
        self.clock = pg.time.Clock()
        self.error_info = None
        self.screen = pg.display.get_surface()
        self.robot = Robot()
        self.arena = Arena(self.robot)
        self.keys = pg.key.get_pressed()
        self.teleop = False
        pg.font.init()
        self.screen.blit(self.arena.image, (0,0))

        self.data = Data(self)
        self.telemetry_plots = TelemetryPlots(self)
        self.telemetry_plots.render_init(self.screen)
        
        self.telemetry_client = TelemetryClient(self.data)

        self.controls = Controls(self)
        self.controls.render_init()

        self.protobuf_readouts = self.data.readouts
        self.previously_clicked_item = None
        self.telemetry_client.start()

    def update_robot_data(self):
        self.robot.update_sprite_angle() 
        self.arena.update_active_segment()

    def render_telemetry(self):
        #  pass
        self.telemetry_plots.render(self.screen)

    def erase(self):
        self.arena.erase(self.screen)

    def render(self):
        self.robot.render(self.screen)
        self.render_telemetry()
        perp_line_image = self.arena.active.generate_desired_heading(self.robot)
        self.screen.blit(perp_line_image, (0,0))
        self.arena.render_active(self.screen)
        self.protobuf_readouts.render()
        pg.display.update()

    def event_loop(self):
        self.keys = pg.key.get_pressed()
        for event in pg.event.get():
            if event.type == pg.QUIT or self.keys[pg.K_ESCAPE]:
                sys.exit()

            if event.type == pg.MOUSEBUTTONDOWN:
                mouse_presses = pg.mouse.get_pressed()
                if mouse_presses[0]: # LEFT MOUSE BUTTON CLICKED
                    pos = pg.mouse.get_pos()
                    if not self.controls.handle_click(pos):
                        if self.telemetry_plots.handle_click(pos):
                            if self.previously_clicked_item:
                                self.previously_clicked_item.set_not_clicked()
                        else:
                            item = self.protobuf_readouts.handle_click(pos)
                            if item:
                                if not self.telemetry_plots.append_plot_if_fits(item):
                                    if self.previously_clicked_item:
                                        self.previously_clicked_item.set_not_clicked()
                                    self.previously_clicked_item = item
                                    item.set_clicked()
                                else:
                                    self.previously_clicked_item = None
                                
        #  TELEOP_POWER = 255
        #  if self.data.cmd.pb.runState is CmdData.RunState.TELEOP:
            #  if self.keys[QWAS['left_fwd']]:
                #  self.data.cmd.pb.leftPower = TELEOP_POWER;
            #  elif self.keys[QWAS['left_back']]:
                #  self.data.cmd.pb.leftPower = -TELEOP_POWER;
            #  else:
                #  self.data.cmd.pb.leftPower = 0;
            #  if self.keys[QWAS['right_fwd']]:
                #  self.data.cmd.pb.rightPower = TELEOP_POWER;
            #  elif self.keys[QWAS['right_back']]:
                #  self.data.cmd.pb.rightPower = -TELEOP_POWER;
            #  else:
                #  self.data.cmd.pb.rightPower = 0;
        #  else:
            #  self.data.cmd.pb.runState = CmdData.RunState.AUTO
            #  self.data.cmd.pb.leftPower = 0
            #  self.data.cmd.pb.rightPower = 0

    #  def toggle_teleop(self):
        #  self.teleop = not self.teleop
        #  if self.data.cmd.pb.runState == CmdData.RunState.TELEOP:
            #  self.data.cmd.pb.runState = CmdData.RunState.AUTO
        #  else:
            #  self.data.cmd.pb.runState = CmdData.RunState.TELEOP

    #  def toggle_robot_enable(self):
        #  self.robot_enabled = not self.robot_enabled
        #  if self.robot_enabled:
            #  if self.teleop:
                #  self.data.cmd.pb.runState = CmdData.RunState.TELEOP
            #  else:
                #  self.data.cmd.pb.runState = CmdData.RunState.AUTO
        #  else:
            #  self.data.cmd.pb.runState = CmdData.RunState.E_STOP

    def main_loop(self):
        while(True):
            self.clock.tick(FPS)
            #  if self.toggle_comms_next_tick:
                #  self._toggle_comms()
            self.event_loop()
            self.erase()
            self.update_robot_data()
            self.render()

def main():
    os.environ['SDL_VIDEO_CENTERED'] = '1'
    pg.init()
    pg.display.set_caption(CAPTION)
    pg.display.set_mode(SCREEN_SIZE)
    app = App()
    app.main_loop()
    pg.quit()
    sys.exit()

if __name__ == "__main__":
    main()
