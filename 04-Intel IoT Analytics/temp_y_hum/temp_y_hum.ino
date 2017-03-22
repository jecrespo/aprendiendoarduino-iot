/*
  Write Temperature and humidity from DHT22 probe with a arduino ethernet shield connection

  Intel IoT Analytics platforn (http://streammyiot.com/) is an analytic IoT platform service that allows you to aggregate, visualize and
  analyze live data streams in the cloud.

  by aprendiendoarduino: www.aprendiendoarduino.com
*/

#include "dht.h"
#include <SPI.h>
#include <Ethernet.h>
#include <simpleNTP.h>
#include <TimeLib.h>        //http://www.arduino.cc/playground/Code/Time

#define DHT22_PIN 7
#define APIKEY "eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJqdGkiOiIzMTBjYjBmYi1lY2Y0LTQxNDgtYTRjZi1hYzNhMWZjZDA4ZjEiLCJpc3MiOiJodHRwOi8vZW5hYmxlaW90LmNvbSIsInN1YiI6IjViNWEwZDU4LWUyZmQtNGEyYy1iMGRmLTk3ZGQwMTQ5YWMzYyIsImV4cCI6IjIwMTctMDMtMjNUMTE6NTI6MjQuMjM0WiIsImFjY291bnRzIjp7IjRkNjM5OGE3LTQ5YWEtNDVmMC04YjUzLTU0ODk2Nzc4YTczNiI6ImFkbWluIn19.oz4YVMpoi0DcrlYQzigOJidEJQXron90y5T4jyLGZCdZIcrsVa8utldYOHlSFSNsUk9z-tJnOoHOoBG72NijJVQLP7HFR1MAmqZnpPhXEZf8dndE0GO0D3J_VZMbapbzjJSanVtCocR8qnr2OP2HvdavDu-lsCA4-0ob1GKPrSk"
#define ACCOUNTID "4d6398a7-49aa-45f0-8b53-54896778a736"
#define DEVICEID "90-a2-da-0d-01-c9"
#define COMPONENTID_TEMP "436e7e74-6771-4898-9057-26932f5eb7e1"
#define COMPONENTID_HUM "436e7e74-6771-4898-9057-26932f5eb7e2"

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xBD};
byte ip[] = {10, 22, 72, 31};
byte DNS[] = {8, 8, 8, 8};
byte gateway[] = {10, 22, 72, 1};
byte subnet[] = {255, 255, 255, 0};

IPAddress server(109, 228, 56, 48); // streammyiot.com IP Address

EthernetClient client;
dht sondaT;

simpleNTP ntp;  //simpleNTP object default NTP server ntp.redimadrid.es (193.145.15.15)

float temp;
float hum;

void setup() {
  Ethernet.begin(mac, ip, DNS, gateway, subnet);
  Serial.begin(9600);
  Serial.println("Requesting Time...");
  unsigned long utc_time = ntp.readNTP();  //UTC time
  setTime(utc_time);
}

void loop() {
  Serial.println(now());
  int chk = sondaT.read22(DHT22_PIN);
  if (chk == DHTLIB_OK) {
    temp = sondaT.temperature;
    hum = sondaT.humidity;
    Serial.print("Temperatura Leida: ");
    Serial.println(temp);
    Serial.print("Humedad Leida: ");
    Serial.println(hum);
    submitData();
  }
  else {
    Serial.print("Error lectura sonda: ");
    Serial.println(chk);
  }
  delay(1000); // update every second
}

void submitData() {
  if (client.connect(server, 80)) {// If theres a successful connection
    Serial.println(F("connected"));
    // Build the data field
    String json = "{\r\n\"on\": " + (String)now() + "000,\r\n\"accountId\": \"" + ACCOUNTID + "\",\r\n\"data\": [\r\n{\r\n\"componentId\": \"" + COMPONENTID_TEMP + "\",\r\n\"on\": " + (String)now() + "000,\r\n\"value\": \"" + (String)temp + "\"\r\n},\r\n{\r\n\"componentId\": \"" + COMPONENTID_HUM + "\",\r\n\"on\": " + (String)now() + "000,\r\n\"value\": \"" + (String)hum + "\"\r\n}\r\n]\r\n}";

    // Make a HTTP request
    client.println("POST /v1/api/data/admin/" + (String)DEVICEID + " HTTP/1.1");
    client.println("Host: 109.228.56.48");
    client.println("Authorization: Bearer " + (String)APIKEY);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println("Connection: close");
    client.println();

    client.println(json);

    Serial.println("POST /v1/api/data/admin/" + (String)DEVICEID + " HTTP/1.1");
    Serial.println("Host: 109.228.56.48");
    Serial.println("Authorization: Bearer " + (String)APIKEY);
    Serial.println("Content-Type: application/json");
    Serial.print("Content-Length: ");
    thisLength = json.length();
    Serial.println(thisLength);
    Serial.println("Connection: close");
    Serial.println();
    Serial.println();

    Serial.println(json);
    Serial.println();
  }
  else {
    // If you didnt get a connection to the server:
    Serial.println(F("connection failed"));
  }

  delay (1000);
  String webString = "";
  if (client.available()) {
    Serial.println("Respuesta del Servidor---->");
    while (client.available()) {
      char c = client.read();
      webString += c;
    }
    Serial.println(webString);

    client.stop();
  }
}

/* ejejmplo json

   {
    "on": 1490186749000,
    "accountId": "4d6398a7-49aa-45f0-8b53-54896778a736",
    "data": [
        {
            "componentId": "436e7e74-6771-4898-9057-26932f5eb7e1",
            "on": 1490186749000,
            "value": "21.7"
        },
        {
            "componentId": "436e7e74-6771-4898-9057-26932f5eb7e2",
            "on": 1490186749000,
            "value": "51.8"
        }
    ]
  }

*/

