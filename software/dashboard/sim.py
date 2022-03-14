import math

PWR_TO_ACCEL = 3
ACC_LIN_TO_ANG = 4

class Sim:
    def __init__(self, pb):
        self.pb = pb

    def simulate(self, leftPower, rightPower, angAcc):
        dt = self.pb.dt
        prevPosX = self.pb.simPosX
        prevPosY = self.pb.simPosY
        prevVelX = self.pb.simVelX
        prevVelY = self.pb.simVelY
        prevAccX = self.pb.simAccX
        prevAccY = self.pb.simAccY
        prevAngXy = self.pb.simAngXy
        prevAngVelXy = self.pb.simAngVelXy
        prevAngAccXy = self.pb.simAngAccXy

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
                
            if curLeftAcc < curRightAcc:
                angAccMag = -angAccMag
                
        elif curLeftAcc < curRightAcc:
            angAccMag = -angAccMag
        
        angAcc = angAccMag*ACC_LIN_TO_ANG

        curAccX = linearAccMag*math.cos(prevAngXy)
        curAccY = linearAccMag*math.sin(prevAngXy)
        
        curVelX = prevVelX + curAccX
        curVelY = prevVelY + curAccY

        curPosX = prevPosX + curVelX*dt + 0.5*curAccX*dt**2
        curPosY = prevPosY + curVelY*dt + 0.5*curAccY*dt**2

        curAngAccXy = angAccMag*ACC_LIN_TO_ANG
        
        curAngVelXy = prevAngVelXy + curAngAccXy

        curAngXy = prevAngXy + curAngVelXy*dt + 0.5*curAngAccXy*dt**2

        self.pb.simPosX = curPosX
        self.pb.simPosY = curPosY
        self.pb.simVelX = curVelX
        self.pb.simVelY = curVelY
        self.pb.simAccX = curAccX
        self.pb.simAccY = curAccY
        self.pb.simAngXy = curAngXy
        self.pb.simAngVelXy = curAngVelXy
        self.pb.simAngAccXy = CurAngAccXy
