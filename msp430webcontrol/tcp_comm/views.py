from django.http import HttpResponse, HttpResponseBadRequest
from django.views.decorators.csrf import csrf_exempt

from msp430.models import *
from tcp_comm.client import push_config

from msp430.models import MSP430

import json

@csrf_exempt
def register(request):
    """When an MSP430 connects to the TCP server this is called"""

    if request.method == 'POST':
        try:
            jreq = json.loads(request.body.decode('UTF-8'))['json']
        except:
            return HttpResponseBadRequest('Unable to parse post json key',
                                          mimetype='application/json')

        # Verify fields exist
        if 'mac' not in jreq or 'ip' not in jreq or 'iface' not in jreq:
            return HttpResponseBadRequest('Does not have required fields',
                                          mimetype='application/json')


        # Update MSP430 model
        default_name = "{0} = {1}".format(jreq['mac'], jreq['ip'])
        msp430_db, created = MSP430.objects.get_or_create(mac_address=jreq['mac'],
                                                          defaults={'current_ip':jreq['ip'],
                                                                    'name':default_name})

        msp430_db.current_ip = jreq['ip']
        msp430_db.online = True
        msp430_db.save()

        def update_iface(model_cls, index_name):
            if index_name in jreq['iface']:
                for iface in jreq['iface'][index_name]:
                    iface_db, created = model_cls.objects.get_or_create(msp430=msp430_db, name=iface['name'],
                        defaults={'io_type':iface['io_type']})

                    iface_db.description = iface['desc']
                    iface_db.possible_choices = json.dumps(iface['choices'])
                    iface_db.save()

        # Update referring interface models
        update_iface(MSP430ReadInterface, 'read')
        update_iface(MSP430WriteInterface, 'write')

        # Send configs to the MSP430
        push_config(msp430_db)


    else:
        return HttpResponse('Not a POST', mimetype='application/json')

    return HttpResponse('ok', mimetype='application/json')

@csrf_exempt
def disconnect(request):
    if request.method == 'POST':
        try:
            jreq = json.loads(request.body.decode('UTF-8'))['json']
        except:
            return HttpResponseBadRequest('Unable to parse post json key', mimetype='application/json')

        # verify fields exist
        if 'mac' not in jreq:
            return HttpResponseBadRequest('Does not have required fields - mac', mimetype='application/json')

        msp430 = MSP430.objects.get(mac_address=jreq['mac'])
        msp430.online = False
        msp430.save()

    return HttpResponse('ok', mimetype='application/json')

