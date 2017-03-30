#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include "Timer.h"

#define DHT22_PIN 7

const char* ssid     = "AndroidAP4628";
const char* password = "password";

WiFiClient client;
Timer t;

boolean estado_boton;

unsigned long myChannelNumber = 250751;
const char * myWriteAPIKey = "APIKEY";

void setup() {
  pinMode(6, INPUT_PULLUP);
  estado_boton = digitalRead(6);
  ThingSpeak.begin(client);
  Serial.begin(9600);
  Serial.println("inicializando red...");
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Iniciando datalogger...");
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

