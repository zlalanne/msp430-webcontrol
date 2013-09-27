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
    def parse_value(value):
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


class AnalogInput(IRead):
    """Maps to ADC read"""

    IO_TYPE = IBase.IO_TYPE_INTEGER

    IO_OPCODE = 2

    IO_CHOICES = (
        (0, 'A0', 'A0'),
        (1, 'A1', 'A1'),
        (2, 'A2', 'A2'),
        (3, 'A3', 'A3'),
        (4, 'A4', 'A4'),
        (5, 'A5', 'A5'),
        (6, 'A6', 'A6'),
        (7, 'A12', 'A12'),
    )

    @staticmethod
    def parse_value(value):
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
        (key, description, pin)
    """
    IO_CHOICES = (
        (1, 'GREEN_LED', "GREENLED"),
        (2, 'RED_LED', "REDLED"),
        (3, "P1_6", "P1_6_OUTPUT" ),
        (4, "P2_7", "P2_7_OUTPUT" ),
        (5, "P8_1", "P8_1_OUTPUT" ),
        (6, "P4_3", "P4_3_OUTPUT" ),
        (7, "P4_0", "P4_0_OUTPUT" ),
        (8, "P3_7", "P3_7_OUTPUT" ),
        (9, "P8_2", "P8_2_OUTPUT" ),
    )

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
