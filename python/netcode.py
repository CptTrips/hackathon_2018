from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor
import numpy as np
import binascii

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

        data_str = binascii.b2a_uu(data)
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

        self.state = self.interpret(data) # change to by-value copy

        print(self.state)

def listen_loop(state, protocol):

    reactor.listenUDP(7777, protocol)

    reactor.run()

if __name__ == "__main__":
    reactor.listenUDP(7777, Echo())
    reactor.run()
