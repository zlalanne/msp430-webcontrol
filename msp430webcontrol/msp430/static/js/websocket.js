function WSClient(url, debug) {
    var self = this;
    this.debug = Boolean(debug);
    this.ws = new WebSocket(url);
    this.ws.onopen = function() {
        self.ws_onopen();
    };
    this.ws.onclose = function() {
        self.ws_onclose();
    };
    this.ws.onmessage = function(msg) {
        self.ws_onmessage(msg);
    };
    this.ws.onerror = function(error) {
        self.ws_onerror(error);
    };

    // callback for errors
    this.onerror = undefined;

    this.datamsgcount_ack = 0;

    this.clientcmds = {
        'CONNECT_MSP430':'msp430_connect',
        'ACK_DATA':'ackdata',
        'WRITE_DATA':'writedata'
    };

    this.servercmds = {
        'MSP430_STATE_CHANGE':'msp430_schange',
        'WRITE_DATA':'writedata'
    };
}

WSClient.prototype.ws_onmessage = function(msg) {
    var parsedMsg = jQuery.parseJSON(msg.data);

    if (this.debug)
        console.log(parsedMsg);

    switch (parsedMsg.cmd) {
        case this.servercmds.MSP430_STATE_CHANGE:
            // for now i don't care, just refresh menu
            switch(parsedMsg.msp430_state)
            {
                case 'drop_stream':
                    // When any MSP430 drops from its streaming state
                    // This occurs when a MSP430 gets reconfigured (admin change)
                    if (this.e_msp430_drop_stream) this.e_msp430_drop_stream(parsedMsg.msp430_mac);
                    break;
                case 'stream':
                    // When any MSP430 begins streaming
                    if (this.e_msp430_stream) this.e_msp430_stream(parsedMsg.msp430_mac);
                    break;
                case 'online':
                    // When any MSP430 comes online
                    if (this.e_msp430_online) this.e_msp430_online(parsedMsg.msp430_mac);
                    break;
                case 'offline':
                    // When any MSP430 goes offline
                    if (this.e_msp430_offline) this.e_msp430_offline(parsedMsg.msp430_mac);
                    break;
                default:
                    break;
            }
            break;
        case this.servercmds.WRITE_DATA:
            this.datamsgcount_ack++;

            for (var key in parsedMsg.read) {
                if ('data_bindings' in window && key in data_bindings) {

                    for (var type in data_bindings[key]) {
                        if (data_bindings[key][type].instances) {
                            for (var index in data_bindings[key][type].instances) {
                                data_bindings[key][type].instances[index].update(parsedMsg.read[key]);
                            }
                        }
                    }
                }
            }
            // TODO: clean up duplicate code
            for (var key in parsedMsg.write) {
                if ('data_bindings' in window && key in data_bindings) {

                    for (var type in data_bindings[key]) {
                        if (data_bindings[key][type].instances) {
                            for (var index in data_bindings[key][type].instances) {
                                data_bindings[key][type].instances[index].update(parsedMsg.write[key]);
                            }
                        }
                    }
                }
            }

            if (this.datamsgcount_ack >= 5) {
                push_msg = {
                    'cmd':this.clientcmds.ACK_DATA,
                    'ack_count':this.datamsgcount_ack
                };
                this.ws.send(JSON.stringify(push_msg));
                this.datamsgcount_ack = 0;
            }

            break;
        default:
            break;
    }
};

WSClient.prototype.ws_onopen = function() {
    if (this.debug) {
        console.log('WS connected');
    }
};

WSClient.prototype.ws_onclose = function() {
    if (this.debug) {
        console.log('WS closed');
    }
};

WSClient.prototype.ws_onerror = function(error) {
    if (this.debug) {
        console.log('Error ' + error);
    }
    if (this.onerror) this.onerror();
};

WSClient.prototype.request_msp430_stream = function(msp430_mac) {
    msg = {
        'cmd':this.clientcmds.CONNECT_MSP430,
        'msp430_mac':msp430_mac
    };
    if (this.debug) {
        console.log(msg);
    }
    this.ws.send(JSON.stringify(msg));
    this.datamsgcount_ack = 0;
    this.bound_msp430_mac = msp430_mac;
};

WSClient.prototype.unregister_msp430 = function() {
    this.bound_msp430_mac = null;
};

WSClient.prototype.send_write_data = function(key, data) {
    msg = {
        'cmd':this.clientcmds.WRITE_DATA,
        'iface_port':key,
        'value':data
    };
    this.ws.send(JSON.stringify(msg));
};
