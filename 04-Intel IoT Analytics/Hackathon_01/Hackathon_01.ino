/*
  Read temperature form sensor and send to the IoT platform

  Also switch on LED from dashboard

  Prework:
  You need iotkit-agent installed and running.
  Device should be activated. (by running: iotkit-admin activate ACTIVATION_CODE)
  Following components should be registered (by running: iotkit-admin register NAME TYPE):
  uptime, cpu_proc, mem_proc, voltage of type e.g. temperature.v1.0
  power of type powerswitch.v1.0
*/

#include <math.h>
#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include "Timer.h"                     //http://github.com/JChristensen/Timer

#define SERVER "109.228.56.48"
#define APIKEY "Bearer APIKEY"

const int TouchPin = 6;
const int ledPin = 5;

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5

Timer t;
IoTkit iotkit;

boolean lastread;

EthernetClient client;

void setup()
{
  Serial.begin(115200);
  iotkit.begin();
  delay(10500);
  pinMode(TouchPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(7, OUTPUT);
  lastread = digitalRead(TouchPin);
  t.every(5000, sendData);

}

void loop()
{
  t.update();

  int sensorValue = digitalRead(TouchPin);
  if (sensorValue != lastread)
  {
    if (sensorValue == HIGH) {
      digitalWrite(ledPin, HIGH);
      digitalWrite(7, HIGH);
      Serial.println("ENCIENDO");
    }
    else
    {
      digitalWrite(ledPin, LOW);
      digitalWrite(7, LOW);
      Serial.println("APAGO");
    }
    iotkit.send("onoffButton", sensorValue);
    lastread = sensorValue;
  }

}

void sendData()
{
  int a = analogRead(pinTempSensor);

  float R = 1023.0 / ((float)a) - 1.0;
  R = 100000.0 * R;

  float temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15; //convert to temperature via datasheet ;

  Serial.print("panel_temperature = ");
  Serial.println(temperature);

  iotkit.send("panel_temperature", temperature);

  int sensorValue = analogRead(1);
  Serial.print("solar_radiation = ");
  Serial.println(sensorValue);

  iotkit.send("solar_radiation", sensorValue);

  retriveData();

}

void retriveData() {
  Serial.println("Retrieve Data");
  if (client.connect(SERVER, 80)) {   // If theres a successful connection
    Serial.println(F("connected"));
    // Build the data field
    //String json = "{\"protocol\":\"v2\",\"device\":\"" + DEVICE_ID + "\",\"at\":\"now\",\"data\":{\"door\":\"" + txt + "\"}}";
    String json = "{\"from\": -10,\"targetFilter\": {\"deviceList\": [\"AA-AA-AA-AA-AA-AA\"]},\"metrics\": [{\"id\": \"654c0298-0e0f-4cf8-97d9-af15d23bcb7c\",\"op\": \"none\"}]}";
    Serial.println(json);      // For debugging purpose only

    // Make a HTTP request
    client.println("POST /v1/api/accounts/0671b6c3-f957-423e-949a-32540d3c589d/data/search HTTP/1.1");
    client.println("Host: 109.228.56.48");
    client.print("Authorization: ");
    client.println (APIKEY);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println("Connection: close");
    client.println();

    client.println(json);
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
    //Serial.println(webString);

    if (webString.endsWith("\"points\":[]}]}")){
      Serial.println("No new data");
    }
    else {
      int data = webString.lastIndexOf("\"}]}]}");
      Serial.println(webString.substring(data-1,data));
    }

    client.stop();
  }
}

