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


Django Application
-----------------
* Written for Python 3.3
* Serves the front end user interface to the browser
* Javascript talks to the websocket server to get data about the MSP430s

Current Status
-----------------
* 0.10 Release
    * Using spare laptop as a dummy "MSP430"
    * Browsers can connect and register to the websocket server
    * MSP430s can connect and register to the websocket server and django app
