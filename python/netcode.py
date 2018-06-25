from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
import numpy as np
import binascii
import threading

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


class BuggyIOThread(threading.Thread):
    def __init__(self, protocol):

        super(BuggyIOThread, slf).__init__()

        self.protocol = protocol

        self.stopped = 0

        self.range_data = np.array([1, 1, 1])

    def run(self):
        reactor.listenUDP(7777, self.protocol)

        reactor.run()

        # Loop range messages

        while self.stopped = 0:
            pass


    def stop(self):
        self.stopped = 1

if __name__ == "__main__":
    reactor.listenUDP(7777, Echo())
    reactor.run()
