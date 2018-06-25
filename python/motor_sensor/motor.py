import time
from gpiozero import CamJamKitRobot

class Motors():
    def __init__(self):
        self._leftmotorspeed=1.0
        self._rightmotorspeed=0.5
        self._speedscale=0.6
        self._motorforward = (self._leftmotorspeed * self._speedscale, self._rightmotorspeed * self._speedscale)
        self._motorbackward = (-self._leftmotorspeed * self._speedscale, -self._rightmotorspeed * self._speedscale)
        self._motorright = (self._leftmotorspeed * self._speedscale, -self._rightmotorspeed * self._speedscale)
        self._motorleft = (-self._leftmotorspeed * self._speedscale, self._rightmotorspeed * self._speedscale)
        self._duration_per_distance = 1.0
        self._duration_per_angle = 2.0/90.0
        self._robot = CamJamKitRobot() 
    
    def forward(self, distance=10):
        self._robot.value = self._motorforward
        time.sleep(distance * self._duration_per_distance)
        self._robot.stop()     

    def backward(self, distance=10):
        self._robot.value = self._motorbackward
        time.sleep(distance * self._duration_per_distance)
        self._robot.stop()

    def right(self, angle=90.0):
        self._robot.value = self._motorright
        time.sleep(angle * self._duration_per_angle)
        self.stop()
	
    def left(self, angle=90.0):
        self._robot.value = self._motorleft
        time.sleep(angle * self._duration_per_angle)
        self.stop()

    def cont_forward(self, left=1.0, right=1.0):
        motorforward = (left * self._leftmotorspeed * self._speedscale, right * self._rightmotorspeed * self._speedscale)
        self._robot.value = motorforward

    def cont_backward(self, left=1.0, right=1.0):
        motorbackward = (-left * self._leftmotorspeed * self._speedscale, -right * self._rightmotorspeed * self._speedscale)
        self._robot.value = motorbackward

    def stop(self):
        self._robot.stop()
        
if __name__=="__main__":
	motors = Motors()
	motors.cont_backward(left=0.7,right=1.0)
	time.sleep(3)
	motors.stop()
	
	
    

