/* -----------------------------------------------------------------------------
   Name:        M0Rx
   Purpose:     RC receiver with 'Cortex M0 Feather' board, WiFi
                and PCA9685
   Author:      Bernd Hinze
   github		    https://github.com/monbera/M0Rx
   Created:     25.02.2020
   Copyright:   (c) Bernd Hinze 2020
   Licence:     MIT see https://opensource.org/licenses/MIT
   ----------------------------------------------------------------------------
*/
#include <MyRC.h>
#include <PCA9685.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

#include "cfg.h"
#define VBATPIN A7

int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;
boolean ipset = false;

int cntcycle = 0;
long rssi = 0;
int tmp = 0;

unsigned int localPort = 6100;      // local port to listen on
IPAddress lip;
int localip[4];
IPAddress bcip;
unsigned long sendTime = millis();
unsigned long timeout = millis();
unsigned long FStimeout = 1000;
unsigned long BCcycle = 1000;  // 1 sec
char CodeTable[]  =  {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?'};
char packetBuffer[100]; //buffer to hold incoming data

char  BCBuffer[30] = {0}; // sending buffer 
int contrlbuffer[3 * 16 + 1] = {0}; // control values for 16 Channels
int Conf[16 * CFGlen];
int Fdat[16];

WiFiUDP Udp;


void setup() {
  WiFi.setPins(8, 7, 4, 2);

  #ifdef DEBUG
    Serial.begin(9600);
    while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
    }
  #endif

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    #ifdef DEBUG
      Serial.println("WiFi shield not present");
    #endif
      while (true);
  }

  // attempt to connect to WiFi network:
  status = WiFi.begin(ssid, pass);
  delay(5000);
  while ( status != WL_CONNECTED) {
    #ifdef DEBUG
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
    #endif
      status = WiFi.begin(ssid, pass);
      delay(5000);
  }
  #ifdef DEBUG
    Serial.println("Connected to wifi");
  #endif
  lip = getWiFiStatus();
  for (int i = 0; i < 4 ; i++) {
    localip[i] = lip[i];
  }
  //appendIP (localip, BCBuffer);
  BCTel(CodeTable, localip, BCBuffer);
  #ifdef DEBUG
    Serial.println("");
    Serial.print("BCDuffer  :");
    Serial.println(BCBuffer);
  #endif
  // Broadcast IP berechnen
  bcip = lip;
  bcip[3] = 255;
  #ifdef DEBUG
    Serial.println(bcip);
    Serial.println("\nStarting connection to server...");
  #endif
  Udp.begin(localPort);

  // Preparation of configuration data
  CtrData_init(Conf);
  //config_channels using 'cfg.h'
  config_chan_all(CHcfg, CHcfg_P, Conf);

  // setup PCA9685 board
  Wire.begin();
  Wire.setClock(400000);
  software_reset();
  pwm_init();
  set_pwm_freq(50);
  fail_safe(Conf);
}


void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    IPAddress remoteIp = Udp.remoteIP();
    if (!ipset)
    {
      bcip[3] = remoteIp[3];  // setting the bcip with the correct remote ip
      ipset = true;
    }
    // read the packet into packetBufffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) packetBuffer[len] = 0;
    tmp = teltoarr(packetBuffer, contrlbuffer);
    #ifdef DEBUG
      Serial.println();
      for (int i = 0; i < tmp + 1 ; i++) {
        Serial.print(contrlbuffer[i]);
      }
    #endif
    timeout = millis();
    update(contrlbuffer, Fdat, Conf);
  }
  if ((millis() - timeout) > FStimeout) {
    fail_safe(Conf);
    }
  if ((millis() - sendTime) > BCcycle) {
    Broadcast(BCBuffer);
    cntcycle += 1;
    if (cntcycle > 10) {
      update_V(CodeTable, getVBAT(VBATPIN), BCBuffer);
      cntcycle = 0;
    }
    sendTime = millis();
  }
}

// ------------------------ functions --------------------------------

// read analog value
float getVBAT(int batterypin) {
  float measuredvbat = analogRead(batterypin);
  measuredvbat *= 2;
  measuredvbat *= 3.3; // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
  return measuredvbat;
  #ifdef DEBUG
    Serial.print(" VBat: " );
    Serial.print(measuredvbat);
  #endif
}

// send a reply, to the IP address and port that sent us the packet we received
void Broadcast(char *buf) {
  Udp.beginPacket(bcip, 6000);
  Udp.write(buf);
  Udp.endPacket();
}

void PrintRecvPaket(int pSize) {
  Serial.print("Received packet of size ");
  Serial.println(pSize);
  Serial.print("From ");
}

void PrintRSSI(long val) {
  Serial.print("signal strength (RSSI):");
  Serial.print(val);
  Serial.println(" dBm");
}

IPAddress getWiFiStatus() {
  // print the SSID of the network you're attached to:
  #ifdef DEBUG
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
  #endif
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  long rssi = WiFi.RSSI();
  #ifdef DEBUG
    Serial.print("IP Address: ");
    Serial.println(ip);
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
  #endif
  return ip;
}
