import sys

from twisted.internet import reactor, protocol
from twisted.python import log

from autobahn.websocket import listenWS

from msp430_ws.server_protocol import MSP430ServerProtocol, MSP430SocketServerFactory, WebServerProtocol

WS_PORT = 9000
HTTP_PORT = 8090
SERVER = "192.168.0.174"
DEBUG = True


def main():

    server_url = "ws://%s:%d" % (SERVER, WS_PORT)

    if DEBUG:
        log.startLogging(sys.stdout)

    # WebSocket Factory for communicating with browser
    ws_factory = MSP430SocketServerFactory(server_url, debug=DEBUG,
                                           debugCodePaths=DEBUG)
    ws_factory.protocol = MSP430ServerProtocol

    # TCP Server for communicating with MSP430 and Django Application
    web_factory = protocol.Factory()
    web_factory.protocol = WebServerProtocol

    if DEBUG:
        web_factory.protocol.debug = True

    # Need to know how to communicate both directions
    web_factory.ws_factory = ws_factory
    ws_factory.web_factory = web_factory

    listenWS(ws_factory)
    reactor.listenTCP(HTTP_PORT, web_factory)

    reactor.run()

if __name__ == '__main__':
    main()
