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

#define DHT22_PIN 7
#define DHTTYPE DHT22

char ssid[] = "AndroidAP4628"; //  your network SSID (name)
char pass[] = "password";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

char url[] = "www.aprendiendoarduino.com";

WiFiClient client;

DHT sondaT(DHT22_PIN, DHTTYPE);

unsigned long myChannelNumber = 242341;
const char * myWriteAPIKey = "MIAPIKEY";

void setup() {

  Serial.begin(9600);
  sondaT.begin();

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
  float hum = sondaT.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = sondaT.readTemperature();
  Serial.print("Temperatura Leida: ");
  Serial.println(temp);
  Serial.print("Humedad Leida: ");
  Serial.println(hum);
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, hum);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  delay(20000); // ThingSpeak will only accept updates every 15 seconds.
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
