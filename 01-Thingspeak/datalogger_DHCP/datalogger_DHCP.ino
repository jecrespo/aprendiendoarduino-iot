#include "ThingSpeak.h"
#include <SPI.h>
#include <Ethernet.h>
#include "Timer.h"

#define DHT22_PIN 7

byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x70, 0xYY};	//Sustituir YY por el numero de MAC correcto

EthernetClient client;
Timer t;

boolean estado_boton;

unsigned long myChannelNumber = 250751;
const char * myWriteAPIKey = "APIKEY";

void setup() {
  pinMode(6, INPUT_PULLUP);
  estado_boton = digitalRead(6);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for (;;)
      ;
  }
  else {
    Serial.print("IP asignada por DHCP: ");
    Serial.println(Ethernet.localIP());
  }
  ThingSpeak.begin(client);
  Serial.begin(9600);
  t.every(5000, grabaDatos);
}

void loop() {
  t.update();
  boolean boton = digitalRead(6);
  if (boton != estado_boton) {
    Serial.print("Boton: ");
    Serial.println(boton);
    estado_boton = boton;
    grabaDatos();
  }
}

void grabaDatos() {
  Serial.println("leyendo datos... ");
  boolean boton = digitalRead(6);
  int sensorVal = analogRead(A0);
  float voltage = (sensorVal / 1024.0) * 5.0;
  float temperature = (voltage - 0.5) * 100; //Sonda temperatura kit Arduino
  Serial.print("Temperatura Leida: ");
  Serial.println(temperature);
  int pot = analogRead(A1);
  Serial.print("Potenciometro: ");
  Serial.println(pot);
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, pot);
  ThingSpeak.setField(3, boton);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

