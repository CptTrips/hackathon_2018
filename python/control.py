import numpy as np
import time


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

    while true:

        if in_deadzone(state, deadzone):
            buggy_stop()

        elif in_fwzone(state):
            buggy_go()

        elif in_bwzone(state):
            buggy_reverse()

        elif in_lzone(state):
            buggy_left()

        elif in_rzone(state):
            buggy_right()

        else:
            buggy_stop()

        reset_state(state)

        time.sleep(0.1)




