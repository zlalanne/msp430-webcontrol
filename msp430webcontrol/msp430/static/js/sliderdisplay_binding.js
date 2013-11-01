// Display object has to be named lower case of python model class name
// constructor always takes the id
function sliderdisplay(id, key, wsclient) {

    this.wsclient = wsclient;
    this.key = key;
    this.slider = $('#' + id + ' > input');

    var self = this;

    this.slider.slider()
        .on('slide', function(ev) {
            self.slide(ev);
        });

}

sliderdisplay.prototype.update = function(value) {
    // Nothing to display on slider
}

sliderdisplay.prototype.slide = function(ev) {
    if(this.wsclient) {
        this.wsclient.send_write_data(this.key, ev.value);
    }
}
