#include "SPI.h"
#include "SimplelinkWifi.h"
#include "wifi.h"

#include "Energia.h"

char ssid[] = "MSP430";     //  your network SSID (name) 
IPAddress serverIP(192,168,1,102);
WiFiClient client();
#define TCP_PORT 0xCAFE
int statusConfig = 0;
unsigned char mac[6], i;
String buffer;
char c;
void setup()
{
  // put your setup code here, to run once:
   Serial.begin(9600);  
  // initialize the digital pin as an output.
  // Pin 14 has an LED connected on most Arduino boards:
  
  
  WiFi.setCSpin(P2_2);
  WiFi.setENpin(P6_5);
  WiFi.setIRQpin(P2_0);
  

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(PUSH1, INPUT);
  pinMode(PUSH2, INPUT);
  
  Serial.println("connecting....");
  
  //connect to an open AP
  
  WiFi.begin(ssid);


  Serial.println("WiFi connect success!");
  Serial.print("IP address = ");
  Serial.println(serverIP);
  Serial.print("MAC  address = ");
  nvmem_get_mac_address(mac);
  for (i=0;i<6;i++)  
  {
    Serial.print(mac[i], HEX);
    if (i<5) 
      Serial.print('.');
  }
  Serial.println("-------------------------------");

  client.connect(IPAddress,TCP_PORT);
  while (!client.connnected());
  Serial.println("TCP Client Connected!");
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(2000);
  buffer = "";
  while (Serial.available()){
    c = Serial.read();
    buffer += c;    
  }
  client.print(buffer);
}
