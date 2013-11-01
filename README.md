MSP430 Web Control
=================

Project started to enable controlling an MSP430 over WiFi.

Current development is being done on develop branch and master provides a "stable" version.

Old release information can be found in [here.](HISTORY.md)


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
    * Compressor django plugin for less compiling for user interface
    * Twitter Bootstrap 3.0 for css and making the web interface easy to create
    * Socket for simple TCP communication with the backend

MSP430 Firmware
----------------
* Written for F5529 Launchpad
* Uses CC3000 for wifi connection
* Acts as a node
* Uses:
    * CC3000 library for connecting to WIFI and TCP sockets
    * JSMN library for decoding json from the server
    * Energia libraries for interfaces
    * MSP430 Driver Library for other peripherals

Current Release
-----------------
* 0.51 Release
    * Added javascript slider for web interface
    * Added analog write to msp430 firmware
