import numpy as np
import time
from motor_sensor.motor import Motors
from motor_sensor.sensors import DistanceSensors
import threading


def in_deadzone(state, deadzone):

    return np.linalg.norm(state) < deadzone

def in_fwzone(state):

    return state[0] < state[1] and -state[0] < state[1]

def in_bwzone(state):

    return state[0] > state[1] and -state[0] > state[1]

def in_lzone(state):

    return state[0] < state[1] and -state[0] > state[1]

def in_rzone(state):

    return state[0] > state[1] and -state[0] < state[1]





class ControlThread(threading.Thread):

    def __init__(self, command_queue, range_queue):

        super(ControlThread,self).__init__()

        self.command_queue = command_queue

        self.stop = 0

        self.range_queue = range_queue

    def reset_state(state):

        state[0] = state[1] = 0

    def run(self):

        # Parameters to put in .cfg
        deadzone = 1

        buggy = Motors()

        rangefinder = DistanceSensors()

        while not self.stop:

            new_ranges = rangefinder.distances()

            self.range_queue.put(new_ranges)

            state = self.command_queue.get()

            if in_deadzone(state, deadzone):
                buggy.stop()

            elif in_fwzone(state):
                buggy.forward()

            elif in_bwzone(state):
                buggy.backward()

            elif in_lzone(state):
                buggy.left()

            elif in_rzone(state):
                buggy.right()

            else:
                buggy.stop()

            self.command_queue.task_done()

            print("State reset: {}".format(state))

            time.sleep(0.01)

        print('Control thread stopped')

