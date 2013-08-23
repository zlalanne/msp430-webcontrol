from django.http import HttpResponse, HttpResponseBadRequest
from django.views.decorators.csrf import csrf_exempt

from msp430.models import msp430

import json

_NEW_DEVICE = False


@csrf_exempt
def register(request):
    """When an MSP430 connects to the TCP server this is called"""

    if request.method == u'POST':
        try:
            jreq = json.loads(request.POST['json'])
        except:
            return HttpResponseBadRequest('Unable to parse post json key',
                                          mimetype='application/json')

        # Verify fields exist
        if 'mac' not in jreq or 'ip' not in jreq:
            return HttpResponseBadRequest('Does not have required fields',
                                          mimetype='application/json')


        # Update MSP430 model
        default_name = "{0} = {1}".format(jreq['mac'], jreq['ip'])
        msp430_db, created = msp430.objects.get_or_create(mac_address=jreq['mac'],
                                                          defaults={'current_ip':jreq['ip'],
                                                                    'name':default_name})

        msp430_db.current_ip = jreq['ip']
        msp430_db.online = True
        msp430_db.save()
    else:
        return HttpResponse('Not a POST', mimetype='application/json')

    global _NEW_DEVICE
    _NEW_DEVICE = True
    return HttpResponse('ok', mimetype='application/json')


def new(request):
    """Poll for AJAX to determine if there is a new device"""

    global _NEW_DEVICE
    results = {'new': _NEW_DEVICE}
    _NEW_DEVICE = False
    return HttpResponse(json.dumps(results), mimetype='application/json')
