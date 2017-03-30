#include "ThingSpeak.h"
#include <SPI.h>
#include <Ethernet.h>

#define DHT22_PIN 7

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xBD};
byte ip[] = {10, 22, 72, 31};
byte DNS[] = {8, 8, 8, 8};
byte gateway[] = {10, 22, 72, 1};
byte subnet[] = {255, 255, 255, 0};

EthernetClient client;

unsigned long myChannelNumber = 250751;
const char * myWriteAPIKey = "APIKEY";

void setup() {
  pinMode(6,INPUT_PULLUP);
  Ethernet.begin(mac, ip, DNS, gateway, subnet);
  ThingSpeak.begin(client);
  Serial.begin(9600);
}

void loop() {
  Serial.println("leyendo datos... ");
  int sensorVal = analogRead(A0);
  float voltage = (sensorVal / 1024.0) * 5.0;
  float temperature = (voltage - 0.5) * 100; //Sonda temperatura kit Arduino
  Serial.print("Temperatura Leida: ");
  Serial.println(temperature);
  int pot = analogRead(A1);
  Serial.print("Potenciometro: ");
  Serial.println(pot);
  boolean boton = digitalRead(6);
  Serial.print("Boton: ");
  Serial.println(boton);
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, pot);
  ThingSpeak.setField(3, boton);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  delay(20000); // ThingSpeak will only accept updates every 15 seconds.
}
