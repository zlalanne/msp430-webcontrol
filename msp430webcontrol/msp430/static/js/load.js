var client;

$(document).ready(function() {

    //interface.getAjaxMenu();
    client = new WSClient('ws://' + window.location.hostname + ':9000/', false);
    // the interface needs to be aware of the ws client to delegate data write requests
    interface.wsclient = client;

    interface.msp430_menu_click = function(context) {
        client.unregister_rpi();
        interface.getAjaxDisplays(context.data.mac, function(){
            client.request_msp430_stream(context.data.mac);
        });
    };

    client.e_msp430_online = function(msp430_mac) {
        interface.getAjaxMenu();
        interface.notify('An MSP430 has come online', 'success', 5000);
    };

    client.e_msp430_offline = function(msp430_mac) {
        interface.getAjaxMenu();
        interface.notify('An MSP430 has gone offline', 'error', 5000);
        // our RPI went offline, cleanup
        if (client.bound_msp430_mac && client.bound_msp430_mac == msp430_mac)
        {
            interface.getAjaxDisplays(msp430_mac, function() {

            });
        }
    };

    client.e_msp430_drop_stream = function(msp430_mac) {
        if (client.bound_msp430_mac && client.bound_msp430_mac == msp430_mac)
        {
            interface.notify('The MSP430 has been reconfigured', 'info', 5000);
        }
    };

    client.e_msp430_stream = function(msp430_mac) {
        if (client.bound_msp430_mac && client.bound_msp430_mac == msp430_mac)
        {
            interface.getAjaxDisplays(msp430_mac, function() {
                client.request_msp430_stream(msp430_mac);
            });
        }
    };

    client.onerror = function() {
        interface.notify('Error with websocket connection, is the server running?', 'error');
    };
});
