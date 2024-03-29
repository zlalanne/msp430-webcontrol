import time
import random

class CHPortInUseException(Exception): pass
class CHPortDoesntExistException(Exception): pass

class IBase(object):
    IO_TYPE_BINARY = 'B'
    IO_TYPE_INTEGER = 'I'

    # Default type, override this
    IO_TYPE = IO_TYPE_BINARY

    # Default type, override this
    IO_OPCODE = -1

    # override this, stored value, description
    # stored value must be an integer, desc must be str
    # ex: ((1, 'GPIO1'),)
    IO_CHOICES = (())

    # Override this. This converts the string returned from the msp430 to a
    # proper data type for displaying on th web interface
    @staticmethod
    def parse_input(value):
        raise NotImplementedError, "Need to implement this for the interface"

    # Override this. This converts the data from the web interface to data the
    # msp430 can use
    @staticmethod
    def parse_output(value):
        raise NotImplementedError, "Need to implement this for the interface"

class IRead(IBase):
    """
    Interface you should extend to define interfaces that are
    available to poll for data on the MSP430.
    """

class IWrite(IBase):
    """
    Interface you should extend to implement an MSP430 writable interface
    """

class GPIOInput(IRead):
    """Maps to GPIO read"""

    IO_TYPE = IBase.IO_TYPE_BINARY
    IO_OPCODE = 1

    IO_CHOICES = (
        (44, "GREEN_LED"),
        (43, "RED_LED"),
        (5, "P1_6" ),
        (8, "P2_7" ),
        (11, "P8_1" ),
        (34, "P4_3" ),
        (33, "P4_0" ),
        (32, "P3_7" ),
        (31, "P8_2" ),
    )

class AnalogInput(IRead):
    """Maps to ADC read"""

    IO_TYPE = IBase.IO_TYPE_INTEGER

    IO_OPCODE = 2

    IO_CHOICES = (
        (0, 'A0'),
        (1, 'A1' ),
        (2, 'A2' ),
        (3, 'A3' ),
        (4, 'A4' ),
        (5, 'A5' ),
        (6, 'A6' ),
        (12, 'A12'),
    )

    @staticmethod
    def parse_input(value):
        try:
            return int(value.strip())
        except:
            return 0


class AnalogOutput(IWrite):
    """Maps to ADC output"""

    IO_TYPE = IBase.IO_TYPE_INTEGER

    IO_OPCODE = 3

    IO_CHOICES = (
        (19, 'P2_0'),
        (40, 'P2_5' ),
        (39, 'P2_4' ),
        (38, 'P1_5' ),
        (37, 'P1_4' ),
    )

    @staticmethod
    def parse_input(value):
        try:
            return int(value.strip())
        except:
            return 0


class GPIOOutput(IWrite):
    """Maps to GPIO write"""

    """IO_TYPE is the type of choice the django application will give"""
    IO_TYPE = IBase.IO_TYPE_BINARY

    """IO_OPCODE is the opcode used to tell the MSP430 the type of IO"""
    IO_OPCODE = 0

    """IO_CHOICES lists the choices for this type of IO. Ex:
        (pin, description)
    """
    IO_CHOICES = (
        (44, "GREEN_LED"),
        (43, "RED_LED"),
        (5, "P1_6" ),
        (8, "P2_7" ),
        (11, "P8_1" ),
        (34, "P4_3" ),
        (33, "P4_0" ),
        (32, "P3_7" ),
        (31, "P8_2" ),
    )

    @staticmethod
    def parse_input(value):

        if value == "HIGH":
            return True
        elif value == "LOW":
            return False
        else:
            return value

    @staticmethod
    def parse_output(value):
        return value


def get_interface_desc():
    read_cls = IRead.__subclasses__()
    write_cls = IWrite.__subclasses__()

    ret = {}
    ret['read'] = read_cls
    ret['write'] = write_cls
    return ret
