import math
import pygame as pg
import time
import numpy

TELEOP_SLOW = 100
TELEOP_MEDIUM = 170
TELEOP_FAST = 255
MAX_TELEOP_POWER = 255

PWR_TO_ACCEL = 0.0001
ACC_LIN_TO_ANG = 100

FRICTIONAL_MULTIPLIER = 0.9

def sind(rad):
    return math.sin(math.radians(rad))

def cosd(rad):
    return math.cos(math.radians(rad))

class Sim:
    def __init__(self, pb):
        self.pb = pb
        self.last_tick = time.time()

    def keys_to_motor_power(self, keys):
        TURNING_WHILE_MOVING_POWER_DIFFERENTIAL = 90
        TURNING_WHILE_MOVING_SLOW_SIDE_FACTOR = 0.5 # run the slow side at 0.5 of its normal
        if keys[pg.K_LSHIFT]:
            TELEOP_POWER = TELEOP_SLOW
        elif keys[pg.K_LALT]:
            TELEOP_POWER = TELEOP_FAST
        else:
            TELEOP_POWER = TELEOP_MEDIUM
        if keys[pg.K_w]:
            left_power = TELEOP_POWER
            right_power = TELEOP_POWER
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
        import time
        new_time = time.time()
        dt = new_time - self.last_tick
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
       
        angAccMag = abs(curLeftAcc - curRightAcc)
        

        linearAcc = 0
        if (curLeftAcc >= 0 and curRightAcc >= 0) or (curLeftAcc <= 0 and curRightAcc <= 0):
            linearAccMag = 0
            if abs(curLeftAcc) > (curRightAcc):
                linearAccMag = abs(curLeftAcc) - angAccMag
            else:
                linearAccMag = abs(curRightAcc) - angAccMag
            if curLeftAcc < 0:
                linearAcc = -linearAccMag
            else:
                linearAcc = linearAccMag

        else:
            linearAcc = (curLeftAcc + curRightAcc)/2
            linearAccMag = abs(linearAcc)

        if curLeftAcc < curRightAcc:
            angAccMag = -angAccMag
        
        angAcc = angAccMag*ACC_LIN_TO_ANG
        #  print(f'angAcc: {angAcc}')

        curAccX = linearAccMag*cosd(prevAngXy)
        curAccY = linearAccMag*sind(prevAngXy)
        #  print(f'curAccY: {curAccY}')
        
        curVelX = prevVelX + curAccX
        curVelY = prevVelY + curAccY
        print(f'curVelY: {curAccY}')
        
        curVelX = numpy.dot([curVelX, 0], prevAngVector)
        curVelY = numpy.dot([0, curVelY], prevAngVector)

        curPosX = prevPosX + curVelX*dt + 0.5*curAccX*dt**2
        curPosY = prevPosY + curVelY*dt + 0.5*curAccY*dt**2

        curAngAccXy = angAccMag*ACC_LIN_TO_ANG
        
        curAngVelXy = prevAngVelXy + curAngAccXy

        curAngXy = prevAngXy + curAngVelXy*dt + 0.5*curAngAccXy*dt**2

        self.pb.simPosX = curPosX
        #  print(f'curPosX: {curPosX}')
        #  print(f'curPosY: {curPosY}')
        self.pb.simPosY = curPosY
        self.pb.simVelX = curVelX
        self.pb.simVelY = curVelY
        self.pb.simAccX = curAccX
        self.pb.simAccY = curAccY
        self.pb.simAngXy = curAngXy
        self.pb.simAngVelXy = curAngVelXy
        self.pb.simAngAccXy = curAngAccXy
