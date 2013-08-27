from django.db import models

class IOTypes(object):
    boolean = 'B'
    integer = 'I'

# Create your models here.
class MSP430(models.Model):
    name = models.CharField(max_length=200)

    # In format AA:BB:CC:DD:EE:FF
    # Should be unique identifier for MSP430
    mac_address = models.CharField(max_length=17, primary_key=True)
    current_ip = models.IPAddressField()
    online = models.BooleanField(default=False)

    def __str__(self):
        return self.name


class MSP430Interface(models.Model):

    # Return type for reads, input for writes
    IO_TYPE = (
        ('B', 'Boolean'),
        ('I', 'Integer'),
    )

    name = models.CharField(max_length=200)
    description = models.TextField(blank=True, default='')

    # This is used to determine the appropriate displays
    io_type = models.CharField(choices=IO_TYPE, max_length=1)

    # Stored as a list of choices in json
    possible_choices = models.TextField(default='[]')

    msp430 = models.ForeignKey(MSP430)

    def __unicode__(self):
        return self.name

    class Meta:
        unique_together = (("name", "msp430"),)
        abstract = True


class MSP430ReadInterface(MSP430Interface):
    pass


class MSP430WriteInterface(MSP430Interface):
    pass


class Display(models.Model):

    # Channel or port
    channel_port = models.IntegerField()
    msp430 = models.ForeignKey(MSP430)
    equation = models.CharField(max_length=100, blank=True)
    label = models.CharField(max_length=200, blank=True)

    def __unicode__(self):
        return '%s - %d' % (self.__class__.__name__, self.channel_port)

    class Meta:
        abstract = True

class WriteDisplay(Display):
    interface = models.ForeignKey(MSP430WriteInterface)

class ReadDisplay(Display):
    interface = models.ForeignKey(MSP430ReadInterface)

class NumericDisplay(ReadDisplay):
    io_type = (IOTypes.boolean, IOTypes.integer)

class ProgressBarDisplay(ReadDisplay):
    io_type = (IOTypes.integer,)

class GraphDisplay(ReadDisplay):
    io_type = (IOTypes.integer,)

class ButtonDisplay(WriteDisplay):
    io_type = (IOTypes.boolean,)



"""Connect signals to ws_comm handler for interface change updates"""
from django.db.models.signals import post_save, post_delete
from tcp_comm.client import config_changed_signal
post_save.connect(config_changed_signal, dispatch_uid="uid_post_save_config")
post_delete.connect(config_changed_signal, dispatch_uid="uid_post_del_config")
