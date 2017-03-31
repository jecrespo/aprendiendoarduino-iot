/*
  Write Temperature and humidity from DHT22 probe with a arduino MKR1000

  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize and
  analyze live data streams in the cloud.

  Based on thingspeak arduino library examples

  by aprendiendoarduino: www.aprendiendoarduino.com
*/

#include "ThingSpeak.h"
#include <DHT.h>
#include <WiFi101.h>

#define DHT22_PIN 9
#define DHTTYPE DHT22

const int EchoPin = 8;
const int TriggerPin = 7;

char ssid[] = "AndroidAP4628"; //  your network SSID (name)
char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiClient client;

DHT sondaT(DHT22_PIN, DHTTYPE);

unsigned long myChannelNumber = 251351;
const char * myWriteAPIKey = "MIKEY";

void setup() {

  Serial.begin(9600);
  sondaT.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);

    Serial.println("Connected to wifi");
    printWifiStatus();

    ThingSpeak.begin(client);

  }
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  float hum = sondaT.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = sondaT.readTemperature();
  Serial.print("Temperatura Leida: ");
  Serial.println(temp);
  Serial.print("Humedad Leida: ");
  Serial.println(hum);
  int cm = ping(TriggerPin, EchoPin);
  Serial.print("Distancia: ");
  Serial.println(cm);
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, hum);
  ThingSpeak.setField(3, cm);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  digitalWrite(LED_BUILTIN, LOW);

  delay(15000); // ThingSpeak will only accept updates every 15 seconds.
}

int ping(int TriggerPin, int EchoPin) {
  long duration, distanceCm;

  digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);

  duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos

  distanceCm = duration * 10 / 292 / 2;  //convertimos a distancia, en cm
  return distanceCm;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
