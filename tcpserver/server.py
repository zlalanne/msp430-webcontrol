import socketserver
import time

from webserver.server_protocol import SiteComm
from webserver.user_protocol import UserComm

TCP_HOST = "192.168.0.158"
TCP_PORT = 9990

HTTP_HOST = TCP_HOST

class MSP430TCPServer(socketserver.TCPServer):
    """Extended version of TCPServer that includes objects
    for MSP430 and Website communication
    """

    def __init__(self, site_comm, user_comm, *args, **kwargs):
        self.site_comm = site_comm
        self.user_comm = user_comm

        super(MSP430TCPServer, self).__init__(*args, **kwargs)


class MSP430TCPHandler(socketserver.BaseRequestHandler):
    """The RequestHandler class for our server.

    It is instantiated once per connection to the server, and must
    override the handle() method to implement communication to the
    client.
    """

    def handle(self):

        # First check if data is from an MSP430 or the Website
        if self.client_address[0] == HTTP_HOST:
            self.server.site_comm.handle_data(self)
        else:
            self.server.user_comm.handle_data(self)


if __name__ == "__main__":

    site_comm = SiteComm()
    user_comm = UserComm()

    # Create the TCP server
    tcp_server = MSP430TCPServer(site_comm, user_comm,
                                 (TCP_HOST, TCP_PORT), MSP430TCPHandler)

    print("{0} Server Starts - {1}".format(time.asctime(), TCP_HOST))
    try:
        tcp_server.serve_forever()
    except KeyboardInterrupt:
        pass

    print("{0} Server Stops - {1}".format(time.asctime(), TCP_HOST))
    tcp_server.shutdown()
