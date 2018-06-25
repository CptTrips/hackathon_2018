import threading
from control import control_loop
from netcode import listen_loop


def sensor_loop():
    """Loop for sending ultrasonic range data to controller"""

    pass


def main():

    state = numpy.array([0,0])

    control_thread = threading.Thread(target = control_loop, args=(state,))

    listen_thread = threading.Thread(target = listen, args=(state,))

    control_thread.start()

    listen_thread.start()

    control_thread.join()
    listen_thread.join()
