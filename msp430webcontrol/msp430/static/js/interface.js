function Interface() {
    this.rpi_menu_click = undefined;
    this.wsclient = undefined;
}

Interface.prototype.notify = function(message, type, timeout) {
    var cls_type;
    switch (type) {
        case 'error':
            cls_type = 'alert-danger';
            break;
        case 'danger':
            cls_type = 'alert-danger';
            break;
        case 'success':
            cls_type = 'alert-success';
            break;
        case 'info':
            cls_type = 'alert-info';
            break;
        case 'warning':
            cls_type = 'alert-warning';
            break;
        default:
            cls_type = 'alert-danger';
            break;
    }

    var html = '<div class="alert fade in ' + cls_type + '">';
    html += '<button type="button" class="close" data-dismiss="alert">&times;</button>';
    html += message;
    html += '</div>';
    var newmsg = $(html);
    $('.messagecontainer').empty();
    $('.messagecontainer').append(newmsg);

    if (timeout) {
        setTimeout(function(){
            $('.messagecontainer > .alert').alert('close');
        }, timeout);
    }
};

Interface.prototype.createMSP430list = function(data) {
    var self = this;
    var dropdownul = $('.nav > .dropdown > ul.dropdown-menu');
    dropdownul.empty();

    var li;
    var li_lst = [];
    if (data.length == 0)
    {
        li = $('<li><a href="#" class="offline_msp430">No MSP430s</a></li>');
        li_lst.push(li);
        dropdownul.append(li_lst);
        return null
    }

    for (var i in data) {
        li = $('<li><a href="#">' + data[i]['name'] + '</a></li>');
        if (!data[i]['online'])
            $(li).find('a').addClass('offline_msp430');
        (function(i, li, data){
            $(li).find('a').click(function(e) {
                var context = {'domli':li, 'data':data[i]};
                self.msp430Clicked(context)
                e.preventDefault();
            });
        })(i, li, data, self);
        li_lst.push(li);
    }

    dropdownul.append(li_lst);
    return li_lst;
};

Interface.prototype.msp430Clicked = function(context) {
    // context is {'domli':li, 'data':data[i]}
    if (this.msp430_menu_click) this.msp430_menu_click(context);
};

Interface.prototype.getAjaxMenu = function() {
    var self = this;
    $.getJSON('/user_api/msp430s/', function(data){
        self.menu = self.createMSP430list(data);
    });
};

Interface.prototype.getAjaxDisplays = function(msp430_mac, callback) {
    var self = this;
    $.get('/displays/' + encodeURIComponent(msp430_mac),
        function(data) {
            interface.cleanup();
            $('#displays_container').html(data);
            // construct object instances for data binds
            if ('data_bindings' in window) {
                for (var key in data_bindings) {
                    for (var type in data_bindings[key]) {
                        data_bindings[key][type].instances = [];

                        var ref = window[type];
                        for (var id_index in data_bindings[key][type].ids) {
                            data_bindings[key][type].instances.push(
                                new ref(data_bindings[key][type].ids[id_index], key, self.wsclient)
                            );
                        }
                    }
                }
            }
            if (callback) callback();
        }
    );
};

Interface.prototype.cleanup = function() {
    if ('data_bindings' in window) {
        for (var key in data_bindings) {
            for (var type in data_bindings[key]) {
                for (var instance_index in data_bindings[key][type].instances) {
                    if (data_bindings[key][type].instances[instance_index].close) {
                        data_bindings[key][type].instances[instance_index].close();
                    }
                }
            }
        }
    }
};

var interface = new Interface();
