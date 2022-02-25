import pygame as pg
import os
import sys
import random
import numpy as np
import time

import socket
import time

from proto.nav_data_pb2 import (NavData)
from proto.cmd_data_pb2 import (CmdData)
                           
from proto.imu_data_pb2 import ImuData
from proto.tof_data_pb2 import TofData
from proto.guidance_data_pb2 import GuidanceData
from proto.hms_data_pb2 import HmsData

from robot import Robot
from arena import Arena
from telemetry_plots import TelemetryPlots
from controls import Controls
from protobuf_readouts import ProtobufReadouts

from constants import *

HOST = "192.168.243.111"  # The server's hostname or IP address
PORT = 23  # The port used by the server


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
    def __init__(self, socket):
        self.clock = pg.time.Clock()
        self.screen = pg.display.get_surface()
        self.robot = Robot()
        self.socket = socket
        self.arena = Arena(self.robot)
        self.keys = pg.key.get_pressed()
        self.comms_connected = False # socket was connected yes or no
        self.comms_enabled = COMMS_DEFAULT_ON
        self.toggle_comms_next_tick = False
        pg.font.init()
        self.font = pg.font.SysFont('Arial', 30)
        #  self.textsurface_1 = self.font.render('telemetry graphs go here', False, (255, 255, 255))
        self.screen.blit(self.arena.image, (0,0))
        self.telemetry_plots = TelemetryPlots()
        self.telemetry_plots.render_init(self.screen)
        
        self.controls = Controls(self)

        self.fake_y_vel = 3 # fake data for plot testing
        self.fake_x_vel = 3
        self.temp_ticks = 0
        self.temp_test_pos = (1,5.25)

        self.fake_commands = []

        self.controls.render()

        self.cmd_data = CmdData()
        self.guidance_data = GuidanceData()
        self.nav_data = NavData()
        self.guidance_data = GuidanceData()
        self.hms_data = HmsData()
        self.imu_data = ImuData()
        self.tof_data = [TofData() for i in range(4)]

        self.protobuf_readouts = ProtobufReadouts(self)

    def update_robot_data(self):
        if FAKE:
            clamp = lambda n, minn, maxn: max(min(maxn, n), minn)
            self.fake_y_vel = clamp(self.fake_y_vel+np.random.normal(scale=1)/10, -5, 5)
            self.fake_x_vel = clamp(self.fake_x_vel+np.random.normal(scale=1)/100, -5, 5)
            if self.robot.rect.centery > 5*PIXELS_PER_TILE and self.fake_y_vel > 0:
                self.fake_y_vel -= 0.1*max(abs(self.fake_y_vel),1) if self.robot.rect.bottom-self.arena.rect.bottom>3 else 5
            elif self.robot.rect.centery < 1*PIXELS_PER_TILE and self.fake_y_vel < 0:
                self.fake_y_vel += 0.1*max(abs(self.fake_y_vel),1) if self.robot.rect.top>3 else 5
            self.fake_x_vel += np.random.normal(scale=5)/10
            if self.robot.rect.centerx > 5*PIXELS_PER_TILE and self.fake_x_vel > 0:
                self.fake_x_vel -= 0.1*max(abs(self.fake_x_vel),1) if self.robot.rect.right-self.arena.rect.right>3 else 5
            elif self.robot.rect.centerx < 1*PIXELS_PER_TILE and self.fake_x_vel < 0:
                self.fake_x_vel += 0.1*max(abs(self.fake_x_vel),1) if self.robot.rect.left>3 else 5

            self.robot.angle = np.degrees(np.tan(self.fake_x_vel/self.fake_y_vel))%360
            #  print(f'self.robot_angle: {self.robot.angle}')
            #  print(f'self.fake_x_vel: {self.fake_x_vel}')
            #  print(f'self.fake_y_vel: {self.fake_y_vel}')
            #  self.robot.update_sprite_angle() 

            fake_y_vel_pixels = self.fake_y_vel * PIXELS_PER_TILE * SPF
            fake_x_vel_pixels = self.fake_x_vel * PIXELS_PER_TILE * SPF
            self.robot.rect.move_ip(fake_x_vel_pixels,fake_y_vel_pixels)
            #  print(f'fake_x_vel_pixels: {fake_x_vel_pixels}')
            #  print(f'fake_y_vel_pixels: {fake_y_vel_pixels}')
            #  self.telemetry_plots.y_vel.update_value(self.fake_y_vel)
            #  self.telemetry_plots.x_vel.update_value(self.fake_x_vel)

            self.telemetry_plots.robot_angle.update_value(self.robot.angle)
        elif FAKE_STATIC:
            if self.temp_ticks % 3 == 0:
                self.robot.angle += 1
            if self.temp_ticks % 100 == 0:
                if self.temp_test_pos == (1,5.25):
                    self.temp_test_pos = (0.75,5)
                elif self.temp_test_pos == (0.75,5):
                    self.temp_test_pos = (0.85,5.25)
                elif self.temp_test_pos == (0.85,5.25):
                    self.temp_test_pos = (0.85,5.75)
                elif self.temp_test_pos == (0.85,5.75):
                    self.temp_test_pos = (0.3, 5.3)
                elif self.temp_test_pos == (0.3, 5.3):
                    self.temp_test_pos = (1.25, 5.2)
                else:
                    self.temp_test_pos = (1,5.25)
            self.temp_ticks += 1

            pos = self.temp_test_pos
            #  pos = (1.25,2.75)
            #  pos = (1,3.25)
            #  pos = (1,3.25)
            pos_pixels = tiles_to_pixels(pos)
            #  print(f'pos_pixels: {pos_pixels}')
            self.robot.rect.center = pos_pixels

        elif FAKE_TELEOP:
            pass
        else:
            pass
        self.telemetry_plots.l_power.update_value(self.guidance_data.leftPower)
        self.telemetry_plots.r_power.update_value(self.guidance_data.rightPower)
        #  print(f'self.guidance_data.leftPower: {self.guidance_data.leftPower}')
        self.telemetry_plots.angle_error.update_value(self.robot.angle_error)
        self.telemetry_plots.robot_angle.update_value(self.robot.angle)
        self.robot.update_sprite_angle() 

        self.arena.update_active_segment()

    def toggle_comms(self):
        self.toggle_comms_next_tick = True

    def _toggle_comms(self, force_enable=None):
        self.toggle_comms_next_tick = False
        print("toggle")
        self.comms_enabled = not self.comms_enabled
        just_connected = False
        if force_enable is not None:
            self.comms_enabled = force_enable
        if self.comms_enabled:
            try:
                self.socket.settimeout(COMMS_TIMEOUT)
                self.socket.connect((HOST,PORT))
                self.socket.settimeout(None)
                self.comms_connected = True
                just_connected = True
            except Exception as e:
                print(f"Failed to connect comms: {e}")
                self.comms_connected = False
                self.controls.comms_toggle.toggle(force_enable=False)
                self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        if self.comms_connected and not just_connected:
            self.socket.shutdown(socket.SHUT_RDWR)
            self.socket.close()
            self.comms_connected = False


    def render_telemetry(self):
        #  pass
        self.telemetry_plots.render(self.screen)

    def erase(self):
        # eventually erase previous telemetry data here
        self.arena.erase(self.screen)

    def render(self):
        self.robot.render(self.screen)
        #  self.screen.blit(self.textsurface_1,(900,100))
        self.render_telemetry()
        perp_line_image = self.arena.active.generate_desired_heading(self.robot)
        self.screen.blit(perp_line_image, (0,0))
        self.arena.render_active(self.screen)
        pg.display.update()

    def event_loop(self):
        self.keys = pg.key.get_pressed()
        for event in pg.event.get():
            if event.type == pg.QUIT or self.keys[pg.K_ESCAPE]:
                #  while(True):
                    #  time.sleep(1)
                sys.exit()

            if event.type == pg.MOUSEBUTTONDOWN:
                mouse_presses = pg.mouse.get_pressed()
                if mouse_presses[0]: # LEFT MOUSE BUTTON CLICKED
                    self.controls.handle_click(pg.mouse.get_pos())

        if self.keys[pg.K_RSHIFT] or self.keys[pg.K_LSHIFT]:
            self.cmd_data.runState = CmdData.RunState.TELEOP
            if self.keys[QWAS['left_fwd']]:
                self.cmd_data.teleop.leftPower = 1;
            elif self.keys[QWAS['left_back']]:
                self.cmd_data.teleop.leftPower = -1;
            else:
                self.cmd_data.teleop.leftPower = 0;
            if self.keys[QWAS['right_fwd']]:
                self.cmd_data.teleop.rightPower = 1;
            elif self.keys[QWAS['right_back']]:
                self.cmd_data.teleop.rightPower = -1;
            else:
                self.cmd_data.teleop.rightPower = 0;
        else:
            self.cmd_data.runState = CmdData.RunState.AUTO
            self.cmd_data.teleop.leftPower = 0
            self.cmd_data.teleop.rightPower = 0
            for k,v in DIRECTION_KEYS.items():
                if self.keys[k]:
                    self.robot.rect.centerx += v[0]*TELEOP_SPEED
                    self.robot.rect.centery += v[1]*TELEOP_SPEED

            if self.keys[ROTATION_KEYS['l']]:
                self.robot.angle -= 15
            if self.keys[ROTATION_KEYS['r']]:
                self.robot.angle += 15

    def telemetry(self):
        if self.comms_connected:
            try:
                self.socket.sendall(self.cmd_data.SerializeToString())
                print("Receiving:")
                rx_raw = self.socket.recv(300)
                msgs = [
                    self.nav_data,
                    self.guidance_data,
                    self.hms_data,
                    self.imu_data
                ] + self.tof_data
                #  msgs = [NavData()]

                #  bytes_decoded = 0
                #  total_to_decode = sum([i.ByteSize() for i in msgs])
                #  print(total_to_decode)
                #  print(len(rx_raw))
                print(rx_raw)
                #  print(cmd_data.teleop.leftPower)
                for raw_data, msg in zip(rx_raw.split(b':'),msgs):
                    msg.ParseFromString(raw_data)

                #  print(msgs[0].posX)
                #  print(msgs[5].dist)
            except Exception as e:
                print(f"failed telem: {e}")


    def main_loop(self):
        while(True):
            self.clock.tick(FPS)
            if self.toggle_comms_next_tick:
                self._toggle_comms()
            self.event_loop()
            self.erase()
            self.update_robot_data()
            self.render()
            self.telemetry()

def main():
    os.environ['SDL_VIDEO_CENTERED'] = '1'
    pg.init()
    pg.display.set_caption(CAPTION)
    pg.display.set_mode(SCREEN_SIZE)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        app = App(s)
        if COMMS_DEFAULT_ON:
            app._toggle_comms()
        app.main_loop()
        pg.quit()
        sys.exit()

if __name__ == "__main__":
    main()
