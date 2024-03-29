import requests
import json
import sys
import re

from twisted.internet.error import ConnectionDone
from twisted.internet import protocol, threads, reactor
from twisted.protocols.basic import LineReceiver
from twisted.python import log
from autobahn.websocket import WebSocketServerFactory, WebSocketServerProtocol, HttpException

import settings
import common_protocol
import buffer
import msp430_data.interface
import msp430_data.utility

class WebServerProtocol(LineReceiver):

    def __init__(self):

        self.client = None

    def lineReceived(self, line):

        line = line.strip()

        if self.client is None:
            if self.debug:
                log.msg("WebServerProtocol.lineReceived - No Client type")
                # TODO: This is an untested way to kill the connection. Need
                # to test.
                self.transport.loseConnection()
        else:
            self.client.dataReceived(line)

    def connectionLost(self, reason=ConnectionDone):

        if self.client is None:
            if self.debug:
                log.msg("WebServerProtocol.connectionClose - No Client type")
            return

        self.client.connectionLost(reason)


    def connectionMade(self):
        if self.transport.getPeer().host == settings.SITE_SERVER:
            self.client = WebServerClient(self)
        else:
            self.client = MSP430Client(self)

        self.client.connectionMade()

    def register_msp430(self):
        """This sends a http request to the django appliaction. This effictevely
        enters the MSP430 into the database of the django application. It then
        tells the websocket server to alert the user that a new MSP430 has
        come online"""


        # Need to send MAC and IP of MSP430
        payload = {}
        payload['mac'] = self.client.mac
        payload['ip'] = self.client.protocol.transport.getPeer().host
        payload['iface'] = self.client.iface

        data = {'json': payload}
        try:
            headers = {'Content-type': 'application/json',
                       'Accept': 'text/plain'}
            response = requests.post("http://%s/tcp_comm/register/" % settings.SITE_SERVER_ADDRESS, data=json.dumps(data),
                                     headers=headers)
        except:
            pass

        # TODO: Need to validate response

        # Notify Browsers
        reactor.callFromThread(self.factory.ws_factory.register_msp430_wsite, self)

    def disconnect_msp430(self):
        payload = {}
        payload['mac'] = self.client.mac

        data = {'json':payload}
        try:
            headers = {'Content-type': 'application/json',
                       'Accept': 'text/plain'}
            response = requests.post("http://%s/tcp_comm/disconnect/" % settings.SITE_SERVER_ADDRESS, data=json.dumps(data),
                                     headers=headers)
        except:
            pass

        # Notify Browsers
        reactor.callFromThread(self.factory.ws_factory.disconnect_msp430_wsite, self)

class WebServerClient(common_protocol.ProtocolState):

    def __init__(self, protocol):
        common_protocol.ProtocolState.__init__(self)
        self.protocol = protocol

    def connectionMade(self):
        pass

    def connectionLost(self, reason=ConnectionDone):
        pass

    def dataReceived(self, data):

        data = data.strip()

        try:
            msp430s = json.loads(data)["json"]
            log.msg(msp430s)
            for msp430 in msp430s:
                if self.protocol.factory.ws_factory.debug:
                    log.msg('WebServerClient - Recieved config for MSP430 %s' % msp430['mac'])
        except:
            if self.protocol.factory.ws_factory.debug:
                log.msg('WebServerClient - Error parsing msp430 configs %s' % sys.exc_info())
            return 'error'

        # Delegate the reqest to the WS factory
        self.protocol.factory.ws_factory.config_msp430(msp430)


class ServerState(common_protocol.State):
    def __init__(self, client):
        self.client = client

    def activated(self):
        if self.client.protocol.debug:
            log.msg("%s.activated()" % self.__class__.__name__)

    def deactivated(self):
        if self.client.protocol.debug:
            log.msg("%s.deactivated()" % self.__class__.__name__)

class WebSocketClient(common_protocol.ProtocolState):
    def __init__(self, protocol):
        common_protocol.ProtocolState.__init__(self)
        self.protocol = protocol

    def onMessage(self, msg):
        try:
            state = self.state_stack.pop_wr()
        except IndexError:
            if self.protocol.factory.debug:
                log.msg("%s.onMessage - Received a message in an unknown state, ignored", self.__class__.__name__)
        state.onMessage(msg)

    def onClose(self, wasClean, code, reason):
        pass

    def onOpen(self):
        pass


class UserClient(WebSocketClient):
    """User client related protocol"""

    def __init__(self, protocol):
        WebSocketClient.__init__(self, protocol)
        self.associated_msp430 = None
        self.streaming_buffer_read = None
        self.streaming_buffer_write = None
        self.ackcount = 0
        self.paused = True

    def register_to_msp430(self, msp430_mac):
        # Notify factory we want to unregister if registered first
        self.ackcount = 0
        if self.associated_msp430 is not None:
            self.protocol.factory.unregister_user_to_msp430(self, self.associated_msp430)
        msp430 = self.protocol.factory.get_msp430(msp430_mac)
        if msp430:
            self.streaming_buffer_read = buffer.UpdateDict()
            self.streaming_buffer_write = buffer.UpdateDict()
            self.associated_msp430 = msp430
            self.protocol.factory.register_user_to_msp430(self, self.associated_msp430)
            # begin streaming
            self.resume_streaming()

    def resume_streaming(self):
        self.paused = False
        self.copy_and_send()

    def pause_streaming(self):
        self.paused = True

    def copy_and_send(self):

        if self.ackcount <= -10 or self.paused:
            return

        # copy buffers
        self.protocol.factory.copy_msp430_buffers(self.associated_msp430,
                                                  self.streaming_buffer_read,
                                                  self.streaming_buffer_write)

        if len(self.streaming_buffer_read) > 0 or len(self.streaming_buffer_write) > 0:
            msg = {'cmd':common_protocol.ServerCommands.WRITE_DATA}
            msg['read'] = self.streaming_buffer_read
            msg['write'] = self.streaming_buffer_write
            self.ackcount -= 1
            self.protocol.sendMessage(json.dumps(msg))
            # keep polling until we run out of data
            reactor.callLater(0, self.copy_and_send)
        else:
            # when there's new data resume will be called
            self.pause_streaming()

    def unregister_to_msp430(self):
        self.pause_streaming()
        if self.associated_msp430 is not None:
            self.associated_msp430 = None

    def notifyMSP430State(self, msp430, state):
        if state == 'config':
            if self.associated_msp430 is not msp430:
                return
        msg = {'cmd':common_protocol.ServerCommands.MSP430_STATE_CHANGE, 'msp430_mac':msp430.client.mac, 'msp430_state':state}
        self.protocol.sendMessage(json.dumps(msg))

    def onMessage(self, msg):
        try:
            msg = json.loads(msg)
        except:
            if self.protocol.debug:
                log.msg('UserState.onMessage - JSON error, dropping')
            self.protocol.failConnection()

        if msg['cmd'] == common_protocol.UserClientCommands.CONNECT_MSP430:
            mac = msg['msp430_mac']
            self.register_to_msp430(mac)

        elif msg['cmd'] == common_protocol.UserClientCommands.ACK_DATA:
            ackcount = msg['ack_count']
            self.ackcount += ackcount
            if self.ackcount > -10:
                self.copy_and_send()

        elif msg['cmd'] == common_protocol.UserClientCommands.WRITE_DATA:
            port = msg['iface_port']
            value = msg['value']
            if self.associated_msp430 is not None:
                self.associated_msp430.write_interface_data(port, value)

    def onClose(self, wasClean, code, reason):
        self.protocol.factory.disconnect_user(self)

    def onOpen(self):
        self.protocol.factory.register_user(self)

class TCPClient(common_protocol.ProtocolState):

    def __init__(self, protocol):
        common_protocol.ProtocolState.__init__(self)
        self.protocol = protocol

    def connectionMade(self):
        pass

    def connectionLost(self, reason=ConnectionDone):
        pass

    def dataReceived(self, data):
        try:
            state = self.state_stack.pop_wr()
        except IndexError:
            if self.protocol.factory.debug:
                log.msg("%s.onMessage - Received a message in an unknown state, ignored", self.__class__.__name__)
        state.dataReceived(data)


class MSP430Client(TCPClient):
    def __init__(self, protocol):
        TCPClient.__init__(self, protocol)

    def connectionMade(self):
        self.push_state(MSP430RegisterState(self))

    def connectionLost(self, reason=ConnectionDone):
        # If we're registered remove ourselves from active client list
        if hasattr(self, 'mac'):
            self.protocol.factory.ws_factory.disconnect_msp430(self)
        pass

    def copy_buffers(self, read_buffer, write_buffer):
        try:
            state = self.current_state()
        except IndexError:
            # MSP430 has no states
            return False
        if isinstance(state, MSP430StreamState):

            for key, value in state.read_data_buffer_eq.iteritems():
                read_buffer[key] = value
            for key, value in state.write_data_buffer_eq.iteritems():
                write_buffer[key] = value

            return True
        return False

    def pause_streaming(self):
        try:
            state = self.current_state()
        except IndexError:
            # MSP430 has no states
            return False

        if isinstance(state, MSP430StreamState):
            state.pause_streaming()
            return True
        return False

    def resume_streaming(self):
        try:
            state = self.current_state()
        except IndexError:
            # MSP430 has no states
            return False

        if isinstance(state, MSP430StreamState):
            state.resume_streaming()
            return True
        return False

    def write_interface_data(self, key, data):
        try:
            state = self.current_state()
        except IndexError:
            # MSP430 has no states
            return False

        if isinstance(state, MSP430StreamState):
            state.write_interface_data(key, data)
            return True
        return False

    def config_io(self, reads, writes):
        """
        read/writes are lsts of dicts with the following:
            'ch_port':  integer or boolean (check cls req)
            'equation': empty, or python style math
            'cls_name': class name as string, ex) 'ADC'

        Returns True/False for success
        """
        # check the state of the MSP430 client
        try:
            state = self.current_state()
        except IndexError:
            # MSP430 has no states
            return False

        if isinstance(state, MSP430ConfigState):
            # ready to be configured
            # MSP430 was waiting for config
            pass
        elif isinstance(state, MSP430StreamState):
            # MSP430 is being re-configured
            state.drop_to_config(reads, writes)
            # config has to be delegated
            return True
        else:
            # MSP430 can't be put into a config state, fail
            return False

        state = self.current_state()
        # delegate the job to the config state
        return state.config_io(reads, writes)


"""MSP430 client related protocol and states"""

class MSP430RegisterState(ServerState):
    def __init__(self, client):
        super(MSP430RegisterState, self).__init__(client)
        self.registered = False
        self.re_message_count = 0


    def dataReceived(self, data):

        if self.re_message_count == 0 and not self.registered:

            # MSP430 is requesting to register
            # TODO: Add some MSP430 authentication here

            if data == common_protocol.ServerCommands.REGISTER:
                self.re_message_count += 1
                if self.client.protocol.debug:
                    log.msg("MSP430Client.dataReceived - Registration Request")
                self.client.protocol.sendLine(common_protocol.ServerCommands.ACK)
            else:
                self.client.protocol.sendLine(common_protocol.ServerCommands.NACK)

        elif self.re_message_count == 1 and not self.registered:

            self.client.protocol.sendLine(common_protocol.ServerCommands.ACK)

            def interface_desc(ifaces):
                # List of classes that resemble I/O. Creating a struct based on
                # their names, docstring, choices and I/O type to send to django
                # application.
                ret = []
                for cls in ifaces:
                    name = cls.__name__
                    desc = msp430_data.utility.trim(cls.__doc__)
                    choices = []

                    for choice_pin, choice_desc in cls.IO_CHOICES:
                        choice = {}
                        choice['s'] = choice_pin
                        choice['d'] = choice_desc
                        choices.append(choice)

                    ret.append({'name':name, 'desc':desc, 'choices':choices, 'io_type':cls.IO_TYPE})

                return ret

            self.client.iface = {}
            self.client.interfaces = msp430_data.interface.get_interface_desc()
            for key in self.client.interfaces.iterkeys():
                self.client.iface[key] = interface_desc(self.client.interfaces[key])

            self.client.mac = data
            self.registered = True
            self.re_message_count = 0

            if self.client.protocol.debug:
                log.msg("MSP430Client.dataReceived - Successful Registration")

            self.client.push_state(MSP430ConfigState(self.client))

            # Add to dictionary of clients in the WS factory
            self.client.protocol.factory.ws_factory.register_msp430(self.client)

        else:
            # TODO: Something went wrong
            return

class MSP430ConfigState(ServerState):
    """In this state, the MSP430 is waiting to be configured.
    Server is not required to configure the MSP430 immediately.
    """
    def __init__(self, client):
        super(MSP430ConfigState, self).__init__(client)

    def dataReceived(self, data):

        if data == common_protocol.MSP430ClientCommands.CONFIG_OK:
            log.msg('MSP430ConfigState - MSP430 was configured')
            self.client.push_state(MSP430StreamState(self.client,
                reads=self.config_reads,
                writes=self.config_writes,
                interfaces=self.config_interfaces,
                mapping=self.config_mapping
            ))
        elif data == common_protocol.MSP430ClientCommands.CONFIG_FAIL:
            if self.client.protocol.debug:
                log.msg('MSP430ConfigState - MSP430 failed to configure')
            # TODO: Notify web server

    def config_io(self, reads, writes):
        """
        read/writes are lsts of dicts with the following:
            'ch_port':  integer or boolean (check cls req)
            'equation': empty, or python style math
            'cls_name': class name as string, ex) 'ADC'

        Returns True/False for success
        """
        self.display_reads = reads
        self.display_writes = writes


        # Format IO to store on the server
        def format_io(io_collection):

            # Duplicate equations allowed, duplicate instances not allowed
            instanced_io_dict = {}
            for io in io_collection:
                cls_str = io['cls_name']
                ch_port = io['ch_port']
                equation = io['equation']

                key = 'cls:%s, port:%s' % (cls_str, ch_port)
                if key not in instanced_io_dict:
                    io_new_dict = {'cls_name':cls_str, 'ch_port':ch_port}
                    io_new_dict['equations'] = [equation]
                    instanced_io_dict[key] = io_new_dict
                else:
                    # we can have more then one equation per instance
                    existing_instance = instanced_io_dict[key]
                    equations = existing_instance['equations']
                    if equation not in equations:
                        equations.append(equation)

            return instanced_io_dict

        # Format IO to give to the msp430
        def format_io_msp430(io_collection):

            config_mapping = {}
            msp430_configs = {}
            i = 0
            for key, io in io_collection:

                cls = getattr(msp430_data.interface, io['cls_name'])
                msp430_configs[str(i)] = {'pin': io['ch_port'], 'opcode': cls.IO_OPCODE}
                config_mapping[str(i)] = io

                i += 1

            return msp430_configs, config_mapping

        self.config_reads = format_io(reads)
        self.config_writes = format_io(writes)

        self.config_interfaces, self.config_mapping = format_io_msp430(self.config_reads.items() + self.config_writes.items())

        if self.client.protocol.debug:
            log.msg('MSP430ConfigState - Pushing configs to remote MSP430')

        msg = {'cmd':common_protocol.ServerCommands.CONFIG,
               'payload':self.config_interfaces}

        self.client.protocol.sendLine(json.dumps(msg))


class MSP430StreamState(ServerState):
    """ In this state the MSP430 has been configured and is streaming data"""
    def __init__(self, client, reads, writes, interfaces, mapping):
        super(MSP430StreamState, self).__init__(client)

        # Read/Write configs is used to communicate with the web
        # Interface config is used to communicate to the msp430
        self.config_reads = reads
        self.config_writes = writes
        self.config_interfaces = interfaces
        self.config_mapping = mapping

        # Buffers for storing the evaluated data
        self.write_data_buffer_eq = {}
        self.read_data_buffer_eq = {}

        self.datamsgcount_ack = 0

    def evaluate_eq(self, eq, value):
        if eq != '':
            # TODO: fix security
            x = value
            new_value = eval(eq)
        else:
            new_value = value
        return new_value

    def deactivated(self):
        super(MSP430StreamState, self).deactivated()
        self.client.protocol.factory.ws_factory.notify_clients_msp430_state_change(self.client.protocol, state='drop_stream')

    def activated(self):
        super(MSP430StreamState, self).activated()
        self.client.protocol.factory.ws_factory.notify_clients_msp430_state_change(self.client.protocol, state='stream')

    def dataReceived(self, data):
        try:
            data = json.loads(data)
        except ValueError:
            log.msg("MSP430StreamState.dataReceived - Problem with JSON structure")
            log.msg(data)
            return

        if data['cmd'] == common_protocol.MSP430ClientCommands.DROP_TO_CONFIG_OK:

            # Order here is important, pop first!
            self.client.pop_state()
            self.client.current_state().config_io(self.delegate_config_reads, self.delegate_config_writes)

        if data['cmd'] == common_protocol.MSP430ClientCommands.DATA:

            self.datamsgcount_ack += 1
            interfaces = data['interfaces']

            for key, value in interfaces.iteritems():

                interface = self.config_mapping[key]
                cls_name = interface["cls_name"]
                pin = interface["ch_port"]

                cls = getattr(msp430_data.interface, cls_name)

                # Convert from raw data string to correct data type
                new_value = cls.parse_input(value)

                # Evaluate equation
                if msp430_data.interface.IWrite in cls.__bases__:

                    for eq in interface["equations"]:
                        new_key = "cls:{}, port:{}, eq:{}".format(cls_name, int(pin), eq)
                        self.write_data_buffer_eq[new_key] = {"calculated" : self.evaluate_eq(eq, new_value),
                                                              "real": new_value}

                elif msp430_data.interface.IRead in cls.__bases__:

                    for eq in interface["equations"]:
                        new_key = "cls:{}, port:{}, eq:{}".format(cls_name, int(pin), eq)
                        self.read_data_buffer_eq[new_key] = self.evaluate_eq(eq, new_value)

            # Notify factory to update listening clients
            if self.datamsgcount_ack >= 5:
                data = {'cmd':common_protocol.ServerCommands.ACK_DATA, 'count':self.datamsgcount_ack}
                self.client.protocol.sendLine(json.dumps(data, sort_keys=True))
                self.datamsgcount_ack = 0

            # Notify factory of new data event
            self.client.protocol.factory.ws_factory.msp430_new_data_event(self.client)

    def resume_streaming(self):
        # Starting to stream again, reset the ack count
        self.datamsgcount_ack = 0
        msg = {'cmd':common_protocol.ServerCommands.RESUME_STREAMING}
        self.client.protocol.sendLine(json.dumps(msg))

    def pause_streaming(self):
        msg = {'cmd':common_protocol.ServerCommands.PAUSE_STREAMING}
        self.client.protocol.sendLine(json.dumps(msg))

    def write_interface_data(self, key, value):

        # Get the class and port from the key
        match = re.match(r'cls:([A-Za-z0-9_]+),\ port:(\d+)', key)

        try:
            cls_name = match.group(1)
            pin = match.group(2)
        except:
            # TODO: add correct exception
            return

        cls = getattr(msp430_data.interface, cls_name)
        payload = {'opcode': cls.IO_OPCODE, 'pin' : pin, 'value' : str(value)}
        msg = {'cmd':common_protocol.ServerCommands.WRITE_DATA,
               'payload': payload}
        self.client.protocol.sendLine(json.dumps(msg))

    def drop_to_config(self, reads, writes):
        # Drop remote MSP430 to config state
        msg = {'cmd':common_protocol.ServerCommands.DROP_TO_CONFIG}
        self.client.protocol.sendLine(json.dumps(msg))
        self.delegate_config_reads = reads
        self.delegate_config_writes = writes


class MSP430ServerProtocol(WebSocketServerProtocol):
    """Base server protocol, instantiates child protocols"""

    def __init__(self):
        self.client = None

    def onConnect(self, connectionRequest):
        """Connection to WebSocket Protocol"""

        def user(headers):
            if self.debug:
                log.msg("MSP430ServerProtocol.onConnect - User connected")
            return UserClient(self)

        # MSP430 won't connect via WebSocket so only option is the client
        paths = {
            '/':user,
        }

        if connectionRequest.path not in paths:
            raise HttpException(httpstatus.HTTP_STATUS_CODE_NOT_FOUND[0],
                                httpstatus.HTTP_STATUS_CODE_NOT_FOUND[1])

        self.client = paths[connectionRequest.path](connectionRequest.headers)

    def onMessage(self, msg, binary):
        """Message received from client"""

        if self.client is None:
            if self.debug:
                log.msg("MSP430ServerProtocol.onMessage - No Client type")
            self.failConnection()

        self.client.onMessage(msg)

    def onOpen(self):
        WebSocketServerProtocol.onOpen(self)
        if self.client is not None:
            self.client.onOpen()

    def onClose(self, wasClean, code, reason):
        """Connect closed, cleanup"""
        # base logs
        WebSocketServerProtocol.onClose(self, wasClean, code, reason)
        if self.client is None:
            if self.debug:
                log.msg("MSP430ServerProtocol.onClose - No Client type")
            return

        self.client.onClose(wasClean, code, reason)


class MSP430SocketServerFactory(WebSocketServerFactory):
    """Manages every MSP430 connected to the server."""

    def __init__(self, *args, **kwargs):

        WebSocketServerFactory.__init__(self, *args, **kwargs)

        # safari
        self.allowHixie76 = True

        # Identify MSP430's by their macs
        # Identify user by peerstr
        self.msp430_clients = {}
        self.user_client = {}

        # Key MSP430 mac, value list of user clients
        self.msp430_clients_registered_users = {}

    def register_user_to_msp430(self, client, msp430):
        if len(self.msp430_clients_registered_users[msp430.mac]) == 0:
            # MSP430 wasn't streaming, start streaming!
            msp430.resume_streaming()
        if client not in self.msp430_clients_registered_users[msp430.mac]:
            self.msp430_clients_registered_users[msp430.mac].append(client)
            if self.debug:
                log.msg('MSP430SocketServerFactory.register_user_to_msp430 msp430:%s user:%s' %
                        (msp430.mac, client.protocol.peerstr))

    def unregister_user_to_msp430(self, client, msp430):
        client.unregister_to_msp430()
        if msp430 is None:
            return
        if msp430.mac in self.msp430_clients_registered_users:
            if client in self.msp430_clients_registered_users[msp430.mac]:
                self.msp430_clients_registered_users[msp430.mac].remove(client)
                if self.debug:
                    log.msg('msp430SocketServerFactory.unregister_user_to_msp430 msp430:%s user:%s' %
                            (msp430.mac, client.protocol.peerstr))
        if msp430.mac not in self.msp430_clients_registered_users or len(self.msp430_clients_registered_users[msp430.mac]) == 0:
            # Pause streaming
            msp430.pause_streaming()

    def msp430_new_data_event(self, msp430):
        # resume streaming on any MSP430s waiting for new data
        for client in self.msp430_clients_registered_users[msp430.mac]:
            client.resume_streaming()

    def copy_msp430_buffers(self, msp430, read_buffer, write_buffer):
        msp430.copy_buffers(read_buffer, write_buffer)

    def get_msp430(self, msp430_mac):
        if msp430_mac in self.msp430_clients:
            return self.msp430_clients[msp430_mac]
        return None

    def notify_clients_msp430_state_change(self, msp430, state='offline'):
        for peerstr, user in self.user_client.iteritems():
            user.notifyMSP430State(msp430, state)

    def register_user(self, user):
        if user.protocol.peerstr not in self.user_client:
            self.user_client[user.protocol.peerstr] = user
            if self.debug:
                log.msg('MSP430SocketServerFactory.register_user %s' % user.protocol.peerstr)

    def disconnect_user(self, user):
        if self.debug:
            log.msg('MSP430SocketServerFactory.disconnect_user %s' % user.protocol.peerstr)
        del self.user_client[user.protocol.peerstr]
        self.unregister_user_to_msp430(user, user.associated_msp430)

    def register_msp430(self, msp430):
        # This is called when the MSP430 has been authenticated with the WS server
        # register on the site server
        msp430.protocol.register_msp430()

        # register locally to the factory
        self.msp430_clients[msp430.mac] = msp430
        self.msp430_clients_registered_users[msp430.mac] = []
        if self.debug:
            log.msg("MSP430SocketServerFactory.register_msp430 - %s registered, %d msp430" % (msp430.mac, len(self.msp430_clients)))

    def register_msp430_wsite(self, msp430):
        """Called after MSP430 has been registed to the website"""
        self.notify_clients_msp430_state_change(msp430, state='online')

    def disconnect_msp430(self, msp430):
        if hasattr(msp430, 'mac'):
            if self.debug:
                log.msg("MSP430SocketServerFactory.disconnect_msp430 - %s msp430 disconnected" % (msp430.mac,))
            reactor.callInThread(msp430.protocol.disconnect_msp430)
            try:
                del self.msp430_clients[msp430.mac]
                del self.msp430_clients_registered_users[msp430.mac]
            except KeyError:
                log.msg(self.msp430_clients)

    def disconnect_msp430_wsite(self, msp430):
        """Called after MSP430 has been disconnected from web server"""
        self.notify_clients_msp430_state_change(msp430, state='offline')

    def config_msp430(self, configs):
        """
        Not thread safe

        configs:
            dict with the following keys:
                'read': lst of port configs
                'write: lst of port configs
                'mac':  '00:00:...'
            port config dict with the following keys:
                'ch_port':  integer or boolean (check cls req)
                'equation': empty, or python style math
                'cls_name': class name as string, ex) 'ADC'

        Return: True/False for success
        """
        # Check if MSP430 is actually an active client
        mac = configs['mac']
        if mac not in self.msp430_clients:
            return False

        msp430_client = self.msp430_clients[mac]
        return msp430_client.config_io(reads=configs['read'], writes=configs['write'])
