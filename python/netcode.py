from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
import numpy as np
import binascii
import threading
import struct

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


    def interpret(self, data):

        # convert bit string to 2D vector

        print(data)

        new_data = [struct.unpack('f', data[0:4]),
                    struct.unpack('f', data[0:4])]

        return new_data

    def datagramReceived(self, data, addr):

        command_data = self.interpret(data)

        if self.command_queue.full():
            self.command_queue.get()
            self.command_queue.task_done()

        self.command_queue.put(command_data)

        print(command_data)

    def send_range(self, range):

        range_bytes = bytearray()
        for r in range:
            range_bytes += bytearray(struct("f", r))

        self.transport.write(range_bytes)




class BuggyIOThread(threading.Thread):
    def __init__(self, protocol, range_queue):

        super(BuggyIOThread, slf).__init__()

        self.protocol = protocol

        self.stopped = 0

        self.range_data = np.array([1, 1, 1])

        self.range_queue = range_queue

    def run(self):
        reactor.listenUDP(7777, self.protocol)

        reactor.run()

        # Loop sending range messages

        while self.stopped = 0:

            if not self.range_queue.empty()

                ranges = self.range_queue.get()

                self.protocol.send(ranges)

                time.sleep(0.3)

    def stop(self):
        self.stopped = 1
        reactor.stop()
        reactor.callFromThread(reactor.stop)

if __name__ == "__main__":
    reactor.listenUDP(7777, Echo())
    reactor.run()
