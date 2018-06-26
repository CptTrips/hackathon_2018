from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
import numpy as np
import binascii
import threading
import struct
import logging
import time

class Echo(DatagramProtocol):

    def datagramReceived(self, data, addr):
        print("Received %r from %s" % (data, addr))
        self.transport.write(data, addr)

class BuggyProtocol(DatagramProtocol):

    def __init__(self, state):

        self.state = state


    def interpret(self, data):

        # binascii the data to a char

        # switch on the char to vectors

        data_str = data.decode('utf-8')
        print(data_str)

        dir_dict = {
            'W' : [0,1],
            'A' : [-1,0],
            'S' : [0,-1],
            'D' : [1,0]
        }

        try:
            new_data = dir_dict[data_str]
        except KeyError:
            new_data = [0,0]

        return new_data

    def datagramReceived(self, data, addr):

        self.state[:] = self.interpret(data)[:]

        print(self.state)


class ListenThread(threading.Thread):
    def __init__(self, protocol):
        super(ListenThread,self).__init__()
        self.protocol = protocol


    def run(self):
        reactor.listenUDP(7777, self.protocol)

        reactor.run()

    def stop(self):
        reactor.stop()
        reactor.callFromThread(reactor.stop)


class BuggyCommandProtocol(DatagramProtocol):

    def __init__(self, command_queue):

        self.command_queue = command_queue

        net_log = logging.getLogger(__name__)

        net_log.setLevel(logging.INFO)

        # Refresh/create file?
        net_log_handler = logging.FileHandler('./network.log')
        net_log_handler.setLevel(logging.INFO)

        net_log.addHandler(net_log_handler)

        timestamp = time.strftime("%H:%M:%S %d-%m-%Y")

        net_log.info('BuggyCommandProtocol created ' + timestamp)

        self.net_log = net_log


    def interpret(self, data):

        # convert bit string to 2D vector

        x = struct.unpack('f', data[0:4])
        y = struct.unpack('f', data[4:8])

        new_data = [x[0],
                    y[0]]

        return new_data

    def datagramReceived(self, data, addr):

        self.net_log.info(''.join('{:02x}'.format(ord(c)) for c in data))

        command_data = self.interpret(data)

        if self.command_queue.full():
            self.command_queue.get()
            self.command_queue.task_done()

        self.command_queue.put(command_data)

        self.net_log.info(command_data)

    def send_range(self, range):

        range_bytes = bytearray()
        for r in range:
            range_bytes += bytearray(struct("f", r))

        self.transport.write(range_bytes)




class BuggyIOThread(threading.Thread):
    def __init__(self, protocol, range_queue):

        super(BuggyIOThread, self).__init__()

        self.protocol = protocol

        self.stopped = 0

        self.range_data = np.array([1, 1, 1])

        self.range_queue = range_queue

    def run(self):
        reactor.listenUDP(7777, self.protocol)

        reactor.run()

        # Loop sending range messages

        while self.stopped == 0:

            if not self.range_queue.empty():

                ranges = self.range_queue.get()

                self.protocol.send(ranges)

                time.sleep(0.3)

        print('IO thread stopped')

    def stop(self):
        self.stopped = 1
        #reactor.stop()
        reactor.callFromThread(reactor.stop)

if __name__ == "__main__":
    reactor.listenUDP(7777, Echo())
    reactor.run()
