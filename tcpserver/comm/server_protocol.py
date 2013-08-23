import http.server
import requests
import json

class SiteComm:
    """To handle requests from website"""

    def register_msp430(self, msp430):

        # Need to send MAC and IP of MSP430
        payload = {}
        payload['mac'] = msp430.mac
        payload['ip'] = msp430.protocol.peer.host

        data = {'json':json.dumps(payload)}

        try:
            headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}
            response = requests.post(url, data=json.dumps(data), headers=headers)
        except:
            pass

    def handle_data(self, request):
        print("Worked")
