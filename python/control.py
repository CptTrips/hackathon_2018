import numpy as np
import time
from motor_sensor.motor import Motors
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


def reset_state(state):

    state[0] = state[1] = 0



class ControlThread(threading.Thread):

    def __init__(self, state):

        super(ControlThread,self).__init__()

        self.state = state

        self.stop = 0


    def run(self):

        # Parameters to put in .cfg
        deadzone = 1

        buggy = Motors()

        while not self.stop:

            if in_deadzone(self.state, deadzone):
                buggy.stop()

            elif in_fwzone(self.state):
                buggy.forward()

            elif in_bwzone(self.state):
                buggy.backward()

            elif in_lzone(self.state):
                buggy.left()

            elif in_rzone(self.state):
                buggy.right()

            else:
                buggy.stop()

            reset_state(self.state)
            print("State reset: {}".format(self.state))

            # Write new ranges to range queue

            time.sleep(0.1)


