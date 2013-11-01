// Display object has to be named lower case of python model class name
// constructor always takes the id
function progressbardisplay(id, key, wsclient) {
    this.domobj = $('#' + id + ' > div.progress > div.progress-bar');
}

progressbardisplay.prototype.update = function(value) {
    console.log(value);
    this.domobj.css('width', parseFloat(value) + '%');
};
