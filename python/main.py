import threading
from control import ControlThread
from control import FineControlThread
from netcode import BuggyIOThread
import netcode
import numpy as np
import time
from Queue import LifoQueue
from twisted.internet import reactor

def sensor_loop():
    """Loop for sending ultrasonic range data to controller"""

    pass


def main():

    command_queue = LifoQueue(1)

    range_queue = LifoQueue(1)

    protocol = netcode.BuggyCommandProtocol(command_queue, range_queue)

    control_thread = FineControlThread(command_queue, range_queue)

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

        control_thread.stop()

        io_thread.join()
        control_thread.join()

        print("Threads stopped gracefully")

if __name__ == "__main__":
    main()
