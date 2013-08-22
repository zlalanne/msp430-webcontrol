CLS
@ECHO off
ECHO Please ensure that your FRAM board is 
ECHO configured correctly and that its USB cable 
ECHO is connected.
PAUSE
@ECHO on

MSP430Flasher.exe -n MSP430G2553 -w "../Binary\BasicWiFiApplication.txt" -z [VCC] -v -g 

@ECHO off
ECHO Please look above to ensure all steps were performed successfully. LED1 of the FRAM
ECHO board should have turned on if the CC3000 EM Module is mounted.
PAUSE
@ECHO on
