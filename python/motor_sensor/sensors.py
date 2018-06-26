from gpiozero import DistanceSensor
import time
#print('defining class ...')
class DistanceSensors():
    def __init__(self):
        self._trigger_pins = (17,22,16)
        self._echo_pins = (18,23,20)
        self._sensors = []
        self._sensors.append(DistanceSensor(
                                  echo=self._echo_pins[0],
                                  trigger=self._trigger_pins[0])
                                     )
        self._sensors.append(DistanceSensor(
                                  echo=self._echo_pins[1],
                                  trigger=self._trigger_pins[1])
                                  )
        self._sensors.append(DistanceSensor(
                                  echo=self._echo_pins[2],
                                  trigger=self._trigger_pins[2])
                                  )
    def distances(self):
        dists = []
        dists.append(self._sensors[0].distance)
        dists.append(self._sensors[1].distance)        
        dists.append(self._sensors[2].distance)
        return dists
        
if __name__=="__main__":
    sensors = DistanceSensors()
    try:
        print("Ultrasonic Measurement")
        while True:
            print('Disances [Front, Left, Right]:', sensors.distances())
            time.sleep(0.2)
    except KeyboardInterrupt:
        print("Exiting")

