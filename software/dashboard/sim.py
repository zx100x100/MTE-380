import math
import pygame as pg
import time
import numpy

TELEOP_SLOW = 100
TELEOP_MEDIUM = 130
TELEOP_FAST = 255
MAX_TELEOP_POWER = 255

#  PWR_TO_ACCEL = 0.001
PWR_TO_ACCEL = 0.1#05
ACC_LIN_TO_ANG = 80

LINEAR_FRICTION = 0.99
ANGULAR_FRICTION = 0.9
MAX_ANGULAR_VELOCITY = 360 * 5

def sign(x):
    return 1 if x>0 else -1

def sind(rad):
    return math.sin(math.radians(rad))

def cosd(rad):
    return math.cos(math.radians(rad))

def constrain(val, maximum):
    if val < 0:
        return max(val, -maximum)
    else:
        return min(val, maximum)

class Sim:
    def __init__(self, pb):
        self.pb = pb
        self.last_tick = time.time()

    def keys_to_motor_power(self, keys):
        TURNING_WHILE_MOVING_POWER_DIFFERENTIAL = 90
        TURNING_WHILE_MOVING_SLOW_SIDE_FACTOR = 0.5 # run the slow side at 0.5 of its normal
        if keys[pg.K_LSHIFT]:
            TELEOP_POWER = TELEOP_SLOW
        elif keys[pg.K_LCTRL]:
            TELEOP_POWER = TELEOP_FAST
        else:
            TELEOP_POWER = TELEOP_MEDIUM
        if keys[pg.K_w]:
            left_power = TELEOP_POWER
            right_power = TELEOP_POWER * 0.91
            if keys[pg.K_d]:
                left_power += TURNING_WHILE_MOVING_POWER_DIFFERENTIAL
                left_power = min(left_power, MAX_TELEOP_POWER)
                right_power *= TURNING_WHILE_MOVING_SLOW_SIDE_FACTOR
            if keys[pg.K_a]:
                right_power += TURNING_WHILE_MOVING_POWER_DIFFERENTIAL
                right_power = min(left_power, MAX_TELEOP_POWER)
                left_power *= TURNING_WHILE_MOVING_SLOW_SIDE_FACTOR
        elif keys[pg.K_s]:
            left_power = -TELEOP_POWER
            right_power = -TELEOP_POWER
            if keys[pg.K_d]:
                left_power -= TURNING_WHILE_MOVING_POWER_DIFFERENTIAL
                left_power = -min(-left_power, MAX_TELEOP_POWER)
                right_power *= TURNING_WHILE_MOVING_SLOW_SIDE_FACTOR
            if keys[pg.K_a]:
                right_power += TURNING_WHILE_MOVING_POWER_DIFFERENTIAL
                right_power = -min(-right_power, MAX_TELEOP_POWER)
                left_power *= TURNING_WHILE_MOVING_SLOW_SIDE_FACTOR
        else:
            if keys[pg.K_d]:
                left_power = TELEOP_POWER
                right_power = -TELEOP_POWER
            elif keys[pg.K_a]:
                left_power = -TELEOP_POWER
                right_power = TELEOP_POWER
            else:
                left_power = 0
                right_power = 0
        return (left_power, right_power)

    def simulate(self, leftPower, rightPower):
        #  print(f'sim.lp:{leftPower},rp:{rightPower}')

        new_time = time.time()
        dt = new_time - self.last_tick
        #  print(f'dt: {dt}')
        self.last_tick = new_time

        prevPosX = self.pb.simPosX
        prevPosY = self.pb.simPosY
        prevVelX = self.pb.simVelX * LINEAR_FRICTION
        prevVelY = self.pb.simVelY * LINEAR_FRICTION
        prevAccX = self.pb.simAccX
        prevAccY = self.pb.simAccY
        prevAngXy = self.pb.simAngXy
        prevAngVelXy = self.pb.simAngVelXy * ANGULAR_FRICTION
        prevAngAccXy = self.pb.simAngAccXy
        prevAngVector = [cosd(prevAngXy), sind(prevAngXy)]

        curLeftAcc = leftPower*PWR_TO_ACCEL
        curRightAcc = rightPower*PWR_TO_ACCEL

        WHEELBASE_TILES = 2/3 * 200 # IDK

        ang_acc = (curLeftAcc-curRightAcc) * WHEELBASE_TILES / 2

        self.pb.simAngAccXy = ang_acc

        self.pb.simAngVelXy = prevAngVelXy + ang_acc * dt

        self.pb.simAngXy = prevAngXy + self.pb.simAngVelXy * dt + ang_acc * dt**2/2

        if self.pb.simAngXy > 360:
            self.pb.simAngXy -= 360
        elif self.pb.simAngXy < -360:
            self.pb.simAngXy += 360

        self.pb.simAngXy = constrain(self.pb.simAngXy, MAX_ANGULAR_VELOCITY)

        avg_acc = (curLeftAcc+curRightAcc)/2

        self.pb.simAccX = avg_acc * cosd(self.pb.simAngXy)
        self.pb.simAccY = avg_acc * sind(self.pb.simAngXy)

        prevVelAvg = (prevVelX**2+prevVelY**2)**0.5

        self.pb.simVelX = prevVelAvg * cosd(self.pb.simAngXy) + self.pb.simAccX * dt
        self.pb.simVelY = prevVelAvg * sind(self.pb.simAngXy) + self.pb.simAccY * dt

        self.pb.simPosX = prevPosX + self.pb.simVelX * dt + self.pb.simAccX * dt**2/2
        self.pb.simPosY = prevPosY + self.pb.simVelY * dt + self.pb.simAccY * dt**2/2


       

        

        #  angAccMag = abs(curLeftAcc - curRightAcc)
        


        #  linearAcc = 0
        #  if (curLeftAcc >= 0 and curRightAcc >= 0) or (curLeftAcc <= 0 and curRightAcc <= 0):
            #  linearAccMag = 0
            #  if abs(curLeftAcc) > (curRightAcc):
                #  linearAccMag = abs(curLeftAcc) - angAccMag
            #  else:
                #  linearAccMag = abs(curRightAcc) - angAccMag
            #  if curLeftAcc < 0:
                #  linearAcc = -linearAccMag
            #  else:
                #  linearAcc = linearAccMag

        #  else:
            #  linearAcc = (curLeftAcc + curRightAcc)/2
            #  linearAccMag = abs(linearAcc)

        #  if curLeftAcc < curRightAcc:
            #  angAccMag = -angAccMag
        
        #  angAcc = angAccMag*ACC_LIN_TO_ANG
        #  #  print(f'angAcc: {angAcc}')

        #  curAccX = linearAcc*cosd(prevAngXy)
        #  #  print(f'curAccX: {curAccX}')
        #  curAccY = linearAcc*sind(prevAngXy)
        #  #  print(f'curAccY: {curAccY}')
        
        #  curVelX = prevVelX + curAccX
        #  curVelY = prevVelY + curAccY
        #  #  print(f'curVelX before dot: {curVelX}')
        
        #  signX = sign(curVelX)
        #  signY = sign(curVelY)

        #  vTotal = (curVelX**2+curVelY**2)**0.5
        
        #  curVelX2 = numpy.dot([curVelX, 0], prevAngVector)
        #  curVelY2 = numpy.dot([0, curVelY], prevAngVector) * sign(curVelY)
        #  if sign(curVelX2) != signX:
            #  curVelX2 *= -1
        #  if sign(curVelY2) != signY:
            #  curVelY2 *= -1

        #  curVelX = curVelX * 0.5 + curVelX2 * 0.5
        #  curVelY = curVelY * 0.5 + curVelY2 * 0.5

        #  #  vTotal2 = (curVelX**2+curVelY**2)**0.5

        #  #  if vTotal2:
            #  #  curVelX *= vTotal/vTotal2
            #  #  curVelY *= vTotal/vTotal2
        
        #  #  print(f'curVelX after dot: {curVelX}')

        #  curPosX = prevPosX + curVelX*dt + 0.5*curAccX*dt**2
        #  curPosY = prevPosY + curVelY*dt + 0.5*curAccY*dt**2

        #  curAngAccXy = angAccMag*ACC_LIN_TO_ANG
        
        #  curAngVelXy = prevAngVelXy + curAngAccXy

        #  curAngXy = prevAngXy + curAngVelXy*dt + 0.5*curAngAccXy*dt**2

        #  self.pb.simPosX = curPosX
        #  #  print(f'curPosX: {curPosX}')
        #  #  print(f'curPosY: {curPosY}')
        #  self.pb.simPosY = curPosY
        #  self.pb.simVelX = curVelX
        #  self.pb.simVelY = curVelY
        #  self.pb.simAccX = curAccX
        #  self.pb.simAccY = curAccY
        #  self.pb.simAngXy = curAngXy
        #  self.pb.simAngVelXy = curAngVelXy
        #  self.pb.simAngAccXy = curAngAccXy
