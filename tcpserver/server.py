import socketserver
import time

from comm.server_state import ServerState

TCP_HOST = "192.168.0.158"
TCP_PORT = 9990

HTTP_HOST = TCP_HOST


class MSP430TCPServer(socketserver.TCPServer):
    """Extended version of TCPServer that includes an object
    to manage the state of the server and handle requests
    """

    def __init__(self, server_state, *args, **kwargs):
        self.server_state = server_state

        super(MSP430TCPServer, self).__init__(*args, **kwargs)


class MSP430TCPHandler(socketserver.BaseRequestHandler):
    """The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):

        # Server state will process the data and determine correct
        # communication
        self.server.server_state.handle_data(self)


if __name__ == "__main__":

    # Create the TCP server
    tcp_server = MSP430TCPServer(ServerState(HTTP_HOST),
                                 (TCP_HOST, TCP_PORT), MSP430TCPHandler)

    print("{0} Server Starts - {1}".format(time.asctime(), TCP_HOST))
    try:
        tcp_server.serve_forever()
    except KeyboardInterrupt:
        pass

    print("{0} Server Stops - {1}".format(time.asctime(), TCP_HOST))
    tcp_server.shutdown()
