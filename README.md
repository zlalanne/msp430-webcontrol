MSP430 Web Control
=================

Project started to enable controlling an MSP430 over WiFi.

Current development is being done on development branch and master provides a "stable" version.


Backend Server
-----------------
* Written for Python 2.7
* Provides Websocket Server
* Provides TCP Server
* Serves to translate data between the MSP430 and the user browsers and the django application
* Uses:
    * Autobahn Python package for Websocket communication
    * Twisted Networking Python package for TCP communication
    * Requests for simple http requests to the Django Application

Django Application
-----------------
* Written for Python 2.7
* Serves the front end user interface to the browser
* Javascript talks to the websocket server to get data about the MSP430s
* Uses:
    * Requests for simple http requests to the backend server
    * Compressor django plugin for less compiling for user interface
    * Twitter Bootstrap 3.0 for css and making the web interface easy to create


Current Releases
-----------------
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


Task List
------------------
- [ ] Get MSP430 to talk over TCP to backend server
- [ ] Add new interfaces to frontend server (digital in, analog in...)
- [ ] Run PEP8 script through the software
