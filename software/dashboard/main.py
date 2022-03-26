import pygame as pg
import os
import sys
import random
import numpy as np
import time

from robot import Robot
from arena import Arena
from proto.hms_and_cmd_data_pb2 import CmdData
from telemetry_plots import TelemetryPlots
from controls import Controls
from data import Data
from telemetry_client import TelemetryClient
from proto.nav_data_pb2 import NavData
import network_setup
from sim import Sim
from recording import Recording
from playback import Playback
from handle_click import handle_click

from constants import *

SERVER_PORT = network_setup.SERVER_PORT
SERVER_IP = network_setup.network_setup()

SIM_USE_GUIDANCE_PID = False

PROP_POWER_THROTTLE_INCREMENT = 20

# 720 x 720 @ 10 PIXELS_PER_INCH
FPS = 10
SPF = 1/FPS
FAKE = True
FAKE_STATIC = False
FAKE_TELEOP = True
TELEOP_SPEED = 20

CAPTION = "yo momma"

class App:
    def __init__(self):
        self.display_update_dt = 0 # temp TODO remove
        self.clock = pg.time.Clock()
        self.error_info = None
        self.screen = pg.display.get_surface()
        self.tof_readouts_initialized = 0
        self.last_connected = False
        self.robot = Robot()
        self.arena = Arena(self.robot, self)
        self.keys = pg.key.get_pressed()
        self.tick_num = 0
        pg.font.init()
        self.screen.blit(self.arena.image, (0,0))

        self.data = Data(self)
        self.telemetry_client = TelemetryClient(self.data, SERVER_IP, SERVER_PORT)
        time.sleep(0.3)
        self.sim = Sim(self.data.cmd.pb)
        self.telemetry_plots = TelemetryPlots(self)
        self.telemetry_plots.render_init(self.screen)
        self.prev_run_state = CmdData.RunState.E_STOP
        

        self.controls = Controls(self)
        self.controls.render_init()

        self.protobuf_readouts = self.data.readouts
        self.previously_clicked_item = None
        self.telemetry_client.start()
        self.recording_to_dirname = None
        self.recording = Recording(self)
        self.playback = Playback(self)
        self.playback.render_init()

        #  self.playback_dirname = 

    def update_robot_data(self):
        if self.data.cmd.pb.runState is CmdData.RunState.SIM and not self.telemetry_client.connected:
            # Display using the dashboard predicted data if we are in SIM mode and not connected to robot
            self.robot.angle = self.data.cmd.pb.simAngXy
            self.robot.rect.centerx = self.data.cmd.pb.simPosX * PIXELS_PER_TILE
            self.robot.rect.centery = self.data.cmd.pb.simPosY * PIXELS_PER_TILE
            self.data.guidance.pb.setpointVel = 4
        else:
            # Display based on nav data for every other state, even if we are in SIM mode
            self.robot.angle = self.data.nav.pb.angXy
            try:
                x = int(self.data.nav.pb.posX * PIXELS_PER_TILE)
                y = int(self.data.nav.pb.posY * PIXELS_PER_TILE)
                self.robot.rect.centerx = x
                self.robot.rect.centery = y
                #  print(f'set robot to nav data. x: {x} | {self.data.nav.pb.posX}, y: {y}, robot.rect.center: {self.robot.rect.center}')
            except:
                #  print("error setting posX/posY from nav data:")
                #  print(f'self.data.nav.pb.posX: {self.data.nav.pb.posX}')
                #  print(f'self.data.nav.pb.posY: {self.data.nav.pb.posY}')
                self.robot.rect.centerx = 0
                self.robot.rect.centery = 0
        self.robot.update_sprite_angle() 
        self.arena.generate_vel_setpoint_indicator(self.data.guidance.pb.setpointVel)

        if self.telemetry_client.connected:
            self.arena.set_active_segment(self.data.guidance.pb.segNum)
        else:
            self.arena.update_active_segment()

    def render_telemetry(self):
        #  pass
        self.telemetry_plots.render(self.screen)

    def erase(self):
        self.arena.erase(self.screen)

    def render(self):
        self.render_telemetry()
        perp_line_image = self.arena.active.generate_desired_heading(self.robot)
        self.screen.blit(perp_line_image, (0,0))
        self.arena.render_active(self.screen)
        self.protobuf_readouts.render()
        self.robot.render(self.screen)
        self.recording.render()
        self.playback.render()
        pg.display.update()

    def event_loop(self):
        self.keys = pg.key.get_pressed()
        for event in pg.event.get():
            if event.type == pg.QUIT or self.keys[pg.K_ESCAPE]:
                self.telemetry_client.kill_thread()
                self.telemetry_client.join()
                pg.quit()
                sys.exit()


            if event.type == pg.MOUSEBUTTONDOWN:
                mouse_presses = pg.mouse.get_pressed()
                handle_click(self, mouse_presses)

        if self.keys[pg.K_r]:
            # start recording shit
            if self.keys[pg.K_LSHIFT]:
                if self.data.recording_to_dirname is not None:
                    self.data.recording_to_dirname = None
                
            else:
                if self.data.recording_to_dirname is None:
                    basedir = os.path.join(os.path.expanduser('~'), 'robot_data')
                    if not os.path.exists(basedir):
                        os.makedirs(basedir)
                    self.data.recording_to_dirname = os.path.join(basedir,self.data.generate_recording_dirname())
        #  if self.keys[pg.K_SPACE]:
            #  # ESTOP:
            #  self.data.cmd.pb.propPower = 0
            #  if self.telemetry_client.connected:
                #  if self.data.cmd.pb.runState is not CmdData.RunState.E_STOP:
                    #  self.controls.start_button.click()
            #  else: # not connected
                #  # reset velocity
                #  self.data.cmd.pb.simVelX = 0
                #  self.data.cmd.pb.simAccX = 0
                #  self.data.cmd.pb.simVelY = 0
                #  self.data.cmd.pb.simAccY = 0
                #  self.data.cmd.pb.simAngVelXy = 0
                #  self.data.cmd.pb.simAngAccXy = 0

        #  if self.keys[pg.K_o]:
            #  self.data.cmd.pb.propPower = max(self.data.cmd.pb.propPower - PROP_POWER_THROTTLE_INCREMENT, 0)
        #  if self.keys[pg.K_p]:
            #  self.data.cmd.pb.propPower = min(self.data.cmd.pb.propPower + PROP_POWER_THROTTLE_INCREMENT, 255)

        if self.data.cmd.pb.runState in (CmdData.RunState.TELEOP, CmdData.RunState.SIM):
            if SIM_USE_GUIDANCE_PID and self.telemetry_client.connected:
                left_power = self.data.guidance.pb.leftTotalPID
                right_power = self.data.guidance.pb.rightTotalPID
            else:
                left_power, right_power = self.sim.keys_to_motor_power(self.keys)
            if self.data.cmd.pb.runState is CmdData.RunState.TELEOP:
                self.data.cmd.pb.leftPower = left_power
                self.data.cmd.pb.rightPower = right_power
            elif self.data.cmd.pb.runState is CmdData.RunState.SIM:
                self.sim.simulate(left_power, right_power)
                self.data.cmd.pb.leftPower = 0
                self.data.cmd.pb.rightPower = 0

    def main_loop(self):
        while(True):
            #  self.data.nav.pb.posX
            # edge detect: switching into sim mode should reset position
            if self.prev_run_state is not CmdData.RunState.SIM and self.data.cmd.pb.runState is CmdData.RunState.SIM:
                # TODO MOVE THIS CODE SOMEWHEREE -------------------------- OVER THE RAINBOW -----------------------------------
                # way up high
                # oh look its kaelan
                # hes pretty high
                # no: it's high how are you!
                self.data.guidance.pb.segNum = 0
                self.arena.set_active_segment(0)
                self.data.cmd.pb.simPosX = 4.45
                self.data.cmd.pb.simPosY = 5.5
                self.data.cmd.pb.simVelY = 0
                self.data.cmd.pb.simVelX = 0
                self.data.cmd.pb.simAngVelXy = 0
                self.data.cmd.pb.simAngXy = 180
                nav_zeros = NavData()

                nav_fields = self.data.nav.pb.DESCRIPTOR.fields_by_name.keys()
                for field in nav_fields:
                    setattr(self.data.nav.pb, field, getattr(nav_zeros, field))
                self.arena.reset_vel_setpoint_indicators()
                
            self.prev_run_state = self.data.cmd.pb.runState
            self.before_tick = time.time()
            self.event_loop()
            self.update_robot_data()
            self.erase()
            self.render()
            self.tick_num += 1
            if self.telemetry_client.connected != self.last_connected:
                print('refreshing connect button!')
                self.controls.connect_button.refresh_state()
                self.controls.start_button.refresh_state()
            self.last_connected = bool(self.telemetry_client.connected)

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
