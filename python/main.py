import threading
from control import ControlThread
from netcode import ListenThread
import netcode
import numpy as np
import time

def sensor_loop():
    """Loop for sending ultrasonic range data to controller"""

    pass


def main():

    state = np.array([0,0])

    protocol = netcode.BuggyProtocol(state)

    control_thread = ControlThread(state)

    listen_thread = ListenThread(protocol)

    try:
        print("Initialising control")
        control_thread.start()

        print("Initialising netcode")
        listen_thread.start()

        print("Buggy online!")

        while 1: time.sleep(1)

    except KeyboardInterrupt:
        print("Exiting")

        listen_thread.stop()

        control_thread.stop = True

        listen_thread.join()
        control_thread.join()

        print("Threads stopped gracefully")

if __name__ == "__main__":
    main()
