from msp430.models import *
from msp430webcontrol import settings
import requests
import json

read_display_cls = ReadDisplay.__subclasses__()
write_display_cls = WriteDisplay.__subclasses__()


def push_config(msp430):
    """Pushes the new configuration to the backend server with the mac
    address of the correct MSP430 to send the data to"""

    msp430_read_ifaces = MSP430ReadInterface.objects.filter(msp430=msp430)
    msp430_write_ifaces = MSP430WriteInterface.objects.filter(msp430=msp430)

    read_displays = ReadDisplay.objects.filter(interface__in=msp430_read_ifaces)
    write_displays = WriteDisplay.objects.filter(interface__in=msp430_write_ifaces)

    msp430_requirements = {}
    msp430_requirements['mac'] = msp430.mac_address

    def add_req(displays, type):
        msp430_requirements[type] = []
        for display in displays:
            display_configs = {
                'cls_name':display.interface.name,
                'ch_port':display.channel_port,
                'equation':display.equation
            }
            msp430_requirements[type].append(display_configs)

    add_req(read_displays, 'read')
    add_req(write_displays, 'write')

    requirements = [msp430_requirements]

    data = {'json':requirements}

    try:
        headers = {'Content-type': 'application/json',
                   'Accept': 'text/plain'}
        response = requests.post("http://%s:%d" % settings.BACKEND_SERVER_IP,
                                 settings.BACKEND_SERVER_HTTP, data=json.dumps(data),
                                 headers=headers)
        # TODO: Add some better checking to see if we acked correctly
        return True
    except:
        return False


def config_changed_signal(sender, **kwargs):
    """Once a configuration has been saved, we need to update the websocket
    server. This is connected to a model save or delete.
    """

    # We need to verify that the sender is a subclass of ReadDisplay or WriteDisplay
    if sender in read_display_cls or sender in write_display_cls:
        # Send update request for the relevant MSP430 configs
        msp430 = kwargs['instance'].msp430
        if msp430.online:
            push_config(msp430)
