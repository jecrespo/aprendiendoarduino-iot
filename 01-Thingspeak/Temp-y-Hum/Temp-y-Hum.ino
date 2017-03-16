/*
  Write Temperature and humidity from DHT22 probe with a arduino ethernet2 shield connection

  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize and
  analyze live data streams in the cloud.

  Based on thingspeak arduino library examples

  by aprendiendoarduino: www.aprendiendoarduino.com
*/

#include "ThingSpeak.h"
#include "dht.h"
#include <SPI.h>
#include <Ethernet2.h>

#define DHT22_PIN 7

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xBD};
byte ip[] = {10, 22, 72, 31};
byte DNS[] = {8, 8, 8, 8};
byte gateway[] = {10, 22, 72, 1};
byte subnet[] = {255, 255, 255, 0};

EthernetClient client;
dht sondaT;

unsigned long myChannelNumber = 242341;
const char * myWriteAPIKey = "MIAPIKEY";

void setup() {
  Ethernet.begin(mac, ip, DNS, gateway, subnet);
  ThingSpeak.begin(client);
  Serial.begin(9600);
}

void loop() {
  int chk = sondaT.read22(DHT22_PIN);
  if (chk == DHTLIB_OK) {
    float temp = sondaT.temperature;
    float hum = sondaT.humidity;
    Serial.print("Temperatura Leida: ");
    Serial.println(temp);
    Serial.print("Humedad Leida: ");
    Serial.println(hum);
    ThingSpeak.setField(1, temp);
    ThingSpeak.setField(2, hum);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  }
  else {
    Serial.print("Error lectura sonda: ");
    Serial.println(chk);
  }

  delay(20000); // ThingSpeak will only accept updates every 15 seconds.
}
