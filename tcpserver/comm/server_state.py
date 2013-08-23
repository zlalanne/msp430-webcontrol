from comm.user_protocol import UserComm
from comm.server_protocol import SiteComm

class ServerState:

    def __init__(self, host='localhost'):
        self.host = host
        self.user_comm = UserComm()
        self.site_comm = SiteComm()

    def handle_data(self, request):

        if request.client_address[0] != self.host:
            data = request.rcv(1024).strip()
            if data == "CAFEBEEF":
                msp = MSP430("01:02:03:04:05:06", request.client_address[0])
                self.site_comm.register_msp430(msp)

        else:
            # Check if we are handling data from server or an MSP430
            if request.client_address[0] == self.host:
                self.site_comm.handle_data(request)
            else:
                self.user_comm.handle_data(request)

class MSP430:

    def __init__(self, mac, ip):
        self.mac = mac
        self.ip = ip
