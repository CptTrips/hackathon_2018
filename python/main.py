import threading
from control import ControlThread
from netcode import BuggyIOThread
import netcode
import numpy as np
import time
from Queue import Queue

def sensor_loop():
    """Loop for sending ultrasonic range data to controller"""

    pass


def main():

    command_queue = Queue(1)

    range_queue = Queue(1)

    protocol = netcode.BuggyCommandProtocol(command_queue)

    control_thread = ControlThread(command_queue, range_queue)

    io_thread = BuggyIOThread(protocol, range_queue)

    try:
        print("Initialising control")
        control_thread.start()

        print("Initialising netcode")
        io_thread.start()

        print("Buggy online!")

        while 1: time.sleep(1)

    except KeyboardInterrupt:
        print("Exiting")

        io_thread.stop()

        control_thread.stop = True

        io_thread.join()
        control_thread.join()

        print("Threads stopped gracefully")

if __name__ == "__main__":
    main()
