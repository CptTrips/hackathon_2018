from gpiozero import DistanceSensor
import time
#print('defining class ...')
class DistanceSensors():
    def __init__(self, sensors_present=(True,True,True)):
        #print('entering __init ...')
        self._trigger_pins = (17,22,24)
        self._echo_pins = (18,23,25)
        self._sensors = []
        for present, index in zip(sensors_present,range(3)):
            #print('present, index: ', present, index)
            if present is True:
                 self._sensors.append(DistanceSensor(
                                      echo=self._echo_pins[index],
                                      trigger=self._trigger_pins[index])
                                     )
        else:    
            self._sensors.append(None)
        #print('leaving init ...')

    def distances(self):
        dists = []
        for sensor in self._sensors:
            if sensor is None:
                dists.append(None)
            else:
                dists.append(sensor.distance)
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

