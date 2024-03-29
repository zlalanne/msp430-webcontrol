from twisted.python import log

class State(object):
    def onMessage(self, msg):
        raise NotImplementedError("Should have implemented this")

    def activated(self):
        """
        When state has become the top of the stack
        """
        self.active = True
        if self.protocol.debug:
            log.msg("%s.activated()" % self.__class__.__name__)

    def deactivated(self):
        self.active = False
        if self.protocol.debug:
            log.msg("%s.deactivated()" % self.__class__.__name__)

    def __init__(self, protocol):
        self.protocol = protocol
        self.active = False


class ServerCommands(object):
    AUTH = 'auth'
    ACK = 'ack'
    NACK = 'nack'
    CONFIG = 'config'
    DROP_TO_CONFIG = 'dropconfig'
    ACK_DATA = 'ackdata'
    RESUME_STREAMING = 'resume'
    PAUSE_STREAMING = 'pause'
    WRITE_DATA = 'writedata'
    REGISTER = 'reg'

    MSP430_STATE_CHANGE = 'msp430_schange'

class MSP430ClientCommands(object):
    CONFIG_OK = 'c_ok'
    CONFIG_FAIL = 'c_fail'
    DROP_TO_CONFIG_OK = 'drop_to_config_ok'
    DATA = 'data'

class UserClientCommands(object):
    CONNECT_MSP430 = 'msp430_connect'
    ACK_DATA = 'ackdata'
    WRITE_DATA = 'writedata'


class StateStack(list):
    def push(self, item):
        self.append(item)

    def pop_wr(self):
        """
        Pop last item without removing it
        """
        return self[-1]


class ProtocolState(object):
    def __init__(self):
        self.state_stack = StateStack()

    def push_state(self, state):
        try:
            self.state_stack.pop_wr().deactivated()
        except IndexError:
            # its the first on the stack
            pass
        self.state_stack.push(state)
        state.activated()

    def pop_state(self):
        self.state_stack.pop().deactivated()
        self.state_stack.pop_wr().activated()

    def current_state(self):
        return self.state_stack.pop_wr()

