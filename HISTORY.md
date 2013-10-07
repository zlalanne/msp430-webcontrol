MSP430 Web Control
=================

Old Releases
-----------------
* 0.31 Release
    * Took out the old MSP430 code

* 0.30 Release
    * MSP430 now acts as a client to the server
    * Added graph and button interface. This is bascially for GPIO Output and Analog Input
    * Still defining communication between MSP430 and server. Might need to simplify json due to CC3000 overhead.
    * Some core functionality still missing. Can't stop a stream state or reconfigure the MSP430 after it's been connect.
    * No support for equations yet

* 0.20 Release
    * Still using spare laptop as a dummy "MSP430"
    * Moved the django application to Python2.7 because I need to use a less compiler and django plugin I wanted to use was only in Python 2.7. Now both the backend and front end interface are both on Python 2.7 which is probably better
    * Implemented state machine for user clients and msp430 clients that connect to backend server
    * Implemented django administration interface that allows a user to add GPIOOutput to a registered MSP430
    * Can successfully send and recieve data to the MSP430. This allows succesful control of a GPIOOutput such as an LED

* 0.10 Release
    * Using spare laptop as a dummy "MSP430"
    * Browsers can connect and register to the websocket server
    * MSP430s can connect and register to the websocket server and django app
