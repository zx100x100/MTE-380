import pygame as pg
import functools
import math

from size_constants import *


LINE_COLOUR = (150,150,150)
LINE_COLOUR2 = (150,150,0)
ACTIVE_SEGMENT_COLOUR = (255,0,0)
NEAREST_DIST_LINE_COLOUR = (255,255,0)
HEADING_ERROR_ARC_COLOUR = (100,200,255)

LINE_THICK=3



class Segment:
    def __init__(self, activated=False):
        self.activated = activated

    def distance(self, point):
        raise NotImplementedError

    def render(self, surface):
        raise NotImplementedError


class CornerCircle(Segment):
    def __init__(self, center, corner, active=False):
        super().__init__(self)
        self.circle_center = [i*PIXELS_PER_TILE for i in center]
        self.corner = corner
        self.radius_tiles = 0.5
        self.active = active
        self.radius_pixels = int(self.radius_tiles*PIXELS_PER_TILE)
        self.image = self.generate_image()
        self.rect = self.image.get_rect()
        if self.corner is Diagonal.TR:
            center_offset = (self.rect.width/2, -self.rect.height/2)
        elif self.corner is Diagonal.BL:
            center_offset = (-self.rect.width/2, self.rect.height/2)
        elif self.corner is Diagonal.TL:
            center_offset = (-self.rect.width/2, -self.rect.height/2)
        else:
            center_offset = (self.rect.width/2, self.rect.height/2)

        self.rect.center = (self.circle_center[0]+center_offset[0],self.circle_center[1]+center_offset[1])

    def completed(self, pos):
        # check if the position indicates having completed the segment

        if self.corner == Diagonal.BL:
            return pos[1] < self.circle_center[1]
        elif self.corner == Diagonal.BR:
            return pos[0] < self.circle_center[0]
        elif self.corner == Diagonal.TL:
            return pos[0] > self.circle_center[0]
        else: #TR
            return pos[1] > self.circle_center[1]
    def distance(self, pos): # position in pixels

        # returns negative if inside circle, positive if outside circle
        return ((pos[0]-self.circle_center[0])**2+(pos[1]-self.circle_center[1])**2)**0.5 - self.radius_pixels

    def get_nearest(self, pos): # get nearest point on the circle
        x1 = pos[0]
        y1 = pos[1]

        xc = self.circle_center[0]
        yc = self.circle_center[1]

        R = self.radius_pixels
        # dist from robot to center of circle
        r = ((pos[0]-self.circle_center[0])**2+(pos[1]-self.circle_center[1])**2)**0.5
        sign = functools.partial(math.copysign, 1)

        # jank code goes here WOOOOOO
        if self.corner == Diagonal.BL:
            x_sign = -sign(x1-xc)
            y_sign = sign(y1-yc)
        elif self.corner == Diagonal.TL:
            x_sign = -sign(x1-xc)
            y_sign = -sign(y1-yc)
        elif self.corner == Diagonal.TR:
            x_sign = sign(x1-xc)
            y_sign = -sign(y1-yc)
        else:
            x_sign = sign(x1-xc)
            y_sign = sign(y1-yc)
        
        if r == 0: # handle div by 0
            return (pos)
        x2 = (x_sign*(x1-xc)/r)*R+xc
        y2 = (y_sign*(y1-yc)/r)*R+yc
        return (x2,y2)

    def generate_image(self):
        image = pg.Surface((self.radius_pixels, self.radius_pixels)).convert_alpha()
        image.fill((0,0,0,0))
        rect = image.get_rect()
        if self.corner is Diagonal.TR:
            center = rect.bottomleft
        elif self.corner is Diagonal.TL:
            center = rect.bottomright
        elif self.corner is Diagonal.BL:
            center = rect.topright
        else:
            center = rect.topleft

        pg.draw.circle(image, ACTIVE_SEGMENT_COLOUR if self.active else LINE_COLOUR2, center, self.radius_pixels, 3)
        return image

    def render(self, screen):
        # TEMP: seperate image generation here so we can check if active (grossssss.....)
        #  screen.blit(self.image, self.rect)
        image = pg.Surface((self.radius_pixels, self.radius_pixels)).convert_alpha()
        image.fill((0,0,0,0))
        rect = image.get_rect()
        if self.corner is Diagonal.TR:
            center = rect.bottomleft
        elif self.corner is Diagonal.TL:
            center = rect.bottomright
        elif self.corner is Diagonal.BL:
            center = rect.topright
        else:
            center = rect.topleft

        pg.draw.circle(image, ACTIVE_SEGMENT_COLOUR if self.active else LINE_COLOUR2, center, self.radius_pixels, 3)
        screen.blit(image, self.rect)

    def generate_desired_heading(self, robot):
        pos = robot.rect.center
        nearest = self.get_nearest(pos)
        nearest_line_width = abs(nearest[0]-self.circle_center[0])
        nearest_line_height = abs(nearest[1]-self.circle_center[1])
        
        #  image = pg.Surface((nearest_line_width,nearest_line_height)).convert_alpha()
        image = pg.Surface((ARENA_SIZE_PIXELS, ARENA_SIZE_PIXELS)).convert_alpha()

        image.fill((0,0,0,0))
        pg.draw.line(image, NEAREST_DIST_LINE_COLOUR, pos, nearest, LINE_THICK)

        desired_heading_arc_rect = pg.Rect(0,0,40,40)
        desired_heading_arc_rect.center = pos
        angle_of_desired_heading = angle_between_positions(pos,nearest)

        robot_angle_error = (angle_of_desired_heading-robot.angle)%360
        heading_angle_pg = angle_to_pg_angle(angle_of_desired_heading)
        robot_angle_pg = angle_to_pg_angle(robot.angle)
        if robot_angle_error>180:
            robot_angle_error -= 360
            start_angle = robot_angle_pg
            end_angle = heading_angle_pg
        else:
            start_angle = heading_angle_pg
            end_angle = robot_angle_pg

        robot.angle_error = robot_angle_error

        desired_heading_arc_rect.center = pos
        pg.draw.arc(image,HEADING_ERROR_ARC_COLOUR,desired_heading_arc_rect,start_angle,end_angle,3)
        return image


class Line(Segment):
    def __init__(self, start, end, active=False):
        self.start = [i*PIXELS_PER_TILE for i in start]
        self.end = [i*PIXELS_PER_TILE for i in end]
        self.horizontal = self.start[1] == self.end[1]
        self.active = active

    def render(self, surface):
        pg.draw.line(surface, ACTIVE_SEGMENT_COLOUR if self.active else LINE_COLOUR, self.start, self.end, LINE_THICK)

    def completed(self, pos):
        if self.horizontal:
            if self.end[0]>self.start[0]:
                return pos[0]>self.end[0]
            return pos[0]<self.end[0]
        if self.end[1]>self.start[1]:
            return pos[1]>self.end[1]
        return pos[1]<self.end[1]


    def distance(self, point):
        # im gonna just be lazy as shit and rely on the fact that the lines
        # are currently guaranteed to be horizontal or vertical to skip the grade 10 calculus
        # of calculating distance to an arbitrary line between (start) and (end)
        if self.horizontal:
            # vertical line, distance is just x value
            return point[0]-start[0]
        else:
            # horizontal line, distance is y value
            return point[1]-start[1]

    def get_nearest(self, pos):
        if self.horizontal:
            return (pos[0], self.start[1])
        else:
            return (self.start[0], pos[1])

    def generate_desired_heading(self, robot):
        pos = robot.rect.center
        nearest = self.get_nearest(pos)
        image = pg.Surface((ARENA_SIZE_PIXELS, ARENA_SIZE_PIXELS)).convert_alpha()

        image.fill((0,0,0,0))
        pg.draw.line(image, NEAREST_DIST_LINE_COLOUR, pos, nearest, LINE_THICK)

        desired_heading_arc_rect = pg.Rect(0,0,40,40)
        desired_heading_arc_rect.center = pos
        angle_of_desired_heading = angle_between_positions(pos,nearest)
        robot_angle_error = (angle_of_desired_heading-robot.angle)%360
        heading_angle_pg = angle_to_pg_angle(angle_of_desired_heading)
        robot_angle_pg = angle_to_pg_angle(robot.angle)
        if robot_angle_error>180:
            robot_angle_error -= 360
            start_angle = robot_angle_pg
            end_angle = heading_angle_pg
        else:
            start_angle = heading_angle_pg
            end_angle = robot_angle_pg

        robot.angle_error = robot_angle_error

        desired_heading_arc_rect.center = pos
        pg.draw.arc(image,HEADING_ERROR_ARC_COLOUR,desired_heading_arc_rect,start_angle,end_angle,3)
        return image

