from django.http import HttpResponse
import json


def tcp_test(request):


    test_json = '{"name": "msp430", "size": 512}'

    return HttpResponse(json.dumps(test_json), mimetype='application/json')
