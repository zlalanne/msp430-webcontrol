import time
import random

class CHPortInUseException(Exception): pass
class CHPortDoesntExistException(Exception): pass

class IBase(object):
    IO_TYPE_BINARY = 'B'
    IO_TYPE_INTEGER = 'I'

    # default state, override this
    IO_TYPE = IO_TYPE_BINARY
    # override this, stored value, description
    # stored value must be an integer, desc must be str
    # ex: ((1, 'GPIO1'),)
    IO_CHOICES = (())

    ALLOW_DUPLICATE_PORTS = False
    channels_in_use = {}

    def __init__(self, ch_port):
        self.ch_port = ch_port

        port_exists = False
        for existing_port, existing_port_name in self.IO_CHOICES:
            if existing_port == ch_port:
                port_exists = True
                break
        if not port_exists:
            raise CHPortDoesntExistException('Port %d does not exist' % ch_port)

        if not self.__class__.ALLOW_DUPLICATE_PORTS:
            if ch_port in self.__class__.channels_in_use:
                raise CHPortInUseException("Channel %d is in use" % ch_port)
            self.__class__.channels_in_use[ch_port] = self

    @classmethod
    def flush(cls):
        """
        Clean out all instances
        """
        for key, value in cls.channels_in_use.items():
            value.close()

    def close(self):
        """
        Because we're probably dealing with IO
        """
        del self.__class__.channels_in_use[self.ch_port]

    @classmethod
    def open(cls, *args, **kwargs):
        """
        Open constructor, idea is to provide a File like interface
        """
        return cls(*args, **kwargs)


class IRead(IBase):
    """
    Interface you should extend to define interfaces that are
    available to poll for data on the RPI.
    """

    def read(self):
        """
        Poll for new data
        Blocks until new data becomes available.
        Returns data
        """
        raise NotImplementedError("Should have implemented this")

    def __iter__(self):
        """
        Generator to repeatedly poll
        """
        while True:
            yield self.read()


class IWrite(IBase):
    """
    Interface you should extend to implement a rpi writable interface
    """
    # This is the default value assumed when no data has been written
    DEFAULT_VALUE = None

    def __init__(self, ch_port):
        super(IWrite, self).__init__(ch_port)
        self.last_written_value = self.DEFAULT_VALUE

    def read(self):
        """
        By default returns the last written state, If no write calls have been made
        it returns the value set by DEFAULT_VALUE
        """
        return self.last_written_value

    def write(self, value):
        self.last_written_value = value


class GPIOOutput(IWrite):
    """Maps to GPIO write"""

    """IO_TYPE is the type of choice the django application will give"""
    IO_TYPE = IBase.IO_TYPE_BINARY

    """IO_OPCODE is the opcode used to tell the MSP430 the type of IO"""
    IO_OPCODE = 0

    """IO_CHOICES lists the choices for this type of IO. Ex:
        (key, description, pin)
    """
    IO_CHOICES = (
        (1, 'GREEN_LED', "GREENLED"),
        (2, 'RED_LED', "REDLED"),
        (3, 'P8.2', "31"),
    )
    DEFAULT_VALUE = False

    def __init__(self, ch_port):
        super(GPIOOutput, self).__init__(ch_port)
        print "Setup the GPIO"

    def write(self, value):
        if value is True:
            print "Set output for GPIO High"
        elif value is False:
            print "Set output for GPIO Low"
        else:
            return
        super(GPIOOutput, self).write(value)

    @staticmethod
    def parse_value(value):
        
        if value == "HIGH":
            return True
        elif value == "LOW":
            return False
        else:
            return value


def get_interface_desc():
    read_cls = IRead.__subclasses__()
    write_cls = IWrite.__subclasses__()

    ret = {}
    ret['read'] = read_cls
    ret['write'] = write_cls
    return ret

