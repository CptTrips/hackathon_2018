import numpy as np
import time
from motor_sensor.motor import Motors


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


def control_loop(state):

    # Parameters to put in .cfg
    deadzone = 1

    buggy = Motors()

    while True:

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

        reset_state(state)

        time.sleep(0.1)




