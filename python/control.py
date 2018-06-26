import numpy as np
import time
from motor_sensor.motor import Motors
from motor_sensor.sensors import DistanceSensors
import threading
from Queue import Queue
import logging
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





class ControlThread(threading.Thread):

    def __init__(self, command_queue, range_queue):

        super(ControlThread,self).__init__()

        self.command_queue = command_queue

        self.stop_q = Queue(1)

        self.range_queue = range_queue

        # control_log
        control_log = logging.getLogger(__name__)

        control_log.setLevel(logging.INFO)

        control_log_handler = logging.FileHandler('./control.log')
        control_log_handler.setLevel(logging.INFO)
        control_log.addHandler(control_log_handler)

        timestamp = time.strftime("%H:%M:%S %d-%m-%Y")

        control_log.info('BuggyCommandProtocol created ' + timestamp)

        self.control_log = control_log


    def reset_state(state):

        state[0] = state[1] = 0

    def run(self):

        # Parameters to put in .cfg
        deadzone = 1

        buggy = Motors()

        rangefinder = DistanceSensors()

        while True:

            # check the stop queue and if the stop message is there break
            if not self.stop_q.empty():
                stop = self.stop_q.get()
                if stop:
                    break

            new_ranges = rangefinder.distances()

            if self.range_queue.full():
                self.range_queue.get()
                self.range_queue.task_done()

            self.range_queue.put(new_ranges)

            if not self.command_queue.empty():
                state = self.command_queue.get()
                self.command_queue.task_done()
            else:
                state = [0, 0]

            print('New state: {}'.format(state))

            if in_deadzone(state, deadzone):
                buggy.stop()
                print('Stop')

            elif in_fwzone(state):
                buggy.forward()
                print('Forward')

            elif in_bwzone(state):
                buggy.backward()
                print('Backward')

            elif in_lzone(state):
                buggy.left()
                print('Left')

            elif in_rzone(state):
                buggy.right()
                print('Right')

            else:
                buggy.stop()
                print('Stop')

            time.sleep(0.05)

        print('Control thread stopped')

    def stop(self):
        self.stop_q.put(True)


class FineControlThread(ControlThread):

    def run(self):
        # Parameters to put in .cfg
        deadzone = 0

        buggy = Motors()

        rangefinder = DistanceSensors()

        while True:

            # check the stop queue and if the stop message is there break
            if not self.stop_q.empty():
                stop = self.stop_q.get()
                if stop:
                    break

            new_ranges = rangefinder.distances()

            if self.range_queue.full():
                self.range_queue.get()
                self.range_queue.task_done()

            self.range_queue.put(new_ranges)

            print("Ranges: {}".format(new_ranges))

            print(self.range_queue.empty())

            if not self.command_queue.empty():
                state = self.command_queue.get()
                self.command_queue.task_done()
            else:
                state = [0, 0]

            print('New state: {}'.format(state))

            if in_deadzone(state, deadzone):
                buggy.stop()
                print('Stop')

            else:

                l = -state[0] + state[1]
                r = state[0] + state[1]

                if abs(l) > 1:
                    l = l / abs(l)
                if abs(r) > 1:
                    r = r / abs(r)

                buggy.cont_forward(l, r)

                print("Input: {}".format(state))


            time.sleep(0.05)

        print('Control thread stopped')


