from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor

class Echo(DatagramProtocol):

    def datagramReceived(self, data, addr):
        print("Received %r from %s" % (data, addr))
        self.transport.write(data, addr)

class BuggyProtocol(DatagramProtocol):

    def __init__(self, state):

        self.state = state


    def interpret(self, data):

        return data

    def datagramReceived(self, data, addr):

        self.state = self.interpret(data)

def listen_loop(state, protocol):

    reactor.listenUDP(7777, protocol)

    reactor.run()

if __name__ == "__main__":
    reactor.listenUDP(7777, Echo())
    reactor.run()
