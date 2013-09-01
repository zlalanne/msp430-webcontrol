from django.http import HttpResponse
from msp430.models import *
import json

def msp430_list(request):
    msp430s = MSP430.objects.all().order_by('-online')

    msp430s_json = []
    for msp430 in msp430s:
        obj_d = {'mac':msp430.mac_address, 'online':msp430.online, 'name':msp430.name}
        msp430s_json.append(obj_d)

    return HttpResponse(json.dumps(msp430s_json), mimetype='application/json')
