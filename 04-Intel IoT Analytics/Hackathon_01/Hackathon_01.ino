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
#include <Wire.h>
#include "rgb_lcd.h"

#define SERVER "109.228.56.48"
#define APIKEY "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJqdGkiOiIzMThmZjdhNS05YjVmLTRhMzAtYTcwZC1iMzk5NTlkOWZkMTYiLCJpc3MiOiJodHRwOi8vZW5hYmxlaW90LmNvbSIsInN1YiI6ImYzMjg1ZjY3LWIzZjQtNDQyNy1hMDVmLTg3NjU3MWJkOWYzNyIsImV4cCI6IjIwMTctMDMtMjdUMTY6NDM6MjYuNDcwWiIsImFjY291bnRzIjp7IjA2NzFiNmMzLWY5NTctNDIzZS05NDlhLTMyNTQwZDNjNTg5ZCI6ImFkbWluIn19.dv6jXCbe6VpRjppLvH19XTnBkC3j_3SYMubNg6NHQmYlE037ic4ga6Lp-oFN2L3OJeHs4f5k-h-Pcs0dQ4g_Tv6Jo8O7lDZXWdZYHTE5hyGmVoLLDIXS5xuoqZx66ZEbaPQYUjmu_K1w7AT8y7SUrLXmsp-vgwr0NP6JIjzP1PQ"

const int TouchPin = 6;
const int ledPin = 5;

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5

Timer t;
IoTkit iotkit;

rgb_lcd lcd;

boolean lastread;

boolean panel_status;

boolean alarma = false;

EthernetClient client;

void setup()
{
  Serial.begin(115200);
  iotkit.begin();
  delay(10500);
  pinMode(TouchPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  lastread = digitalRead(TouchPin);
  t.every(5000, sendData);
  lcd.begin(16, 2);
  panel_status = true;
  iotkit.send("status", panel_status);

}

void loop()
{
  t.update();
  checkParams();

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
  if (alarma == false) {
    retriveData();
    if (panel_status) {
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

      lcd.clear();
      //lcd.write(Serial.read());
      lcd.print("Radiation: ");
      lcd.print(sensorValue);
      lcd.setCursor(0, 1);
      lcd.print("Temp: ");
      lcd.print(temperature);
    }
  }
}

void retriveData() {
  Serial.println("Retrieve Data");
  if (client.connect(SERVER, 80)) {   // If theres a successful connection
    //Serial.println(F("connected"));
    // Build the data field
    //String json = "{\"protocol\":\"v2\",\"device\":\"" + DEVICE_ID + "\",\"at\":\"now\",\"data\":{\"door\":\"" + txt + "\"}}";
    String json = "{\"from\": -100,\"targetFilter\": {\"deviceList\": [\"AA:AA:AA:AA:AA:01\"]},\"metrics\": [{\"id\": \"56af18f5-c9d6-49fc-a4a3-7697f17de655\",\"op\": \"none\"}]}";
    //Serial.println(json);      // For debugging purpose only

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
    int new_panelStatus;
    //Serial.println("Respuesta del Servidor---->");
    while (client.available()) {
      char c = client.read();
      webString += c;
    }
    //Serial.println(webString);

    if (webString.endsWith("\"points\":[]}]}")) {
      //Serial.println("No new data");
    }
    else {
      int data = webString.lastIndexOf("\"}]}]}");
      Serial.print("Last Data: ");
      Serial.println(webString.substring(data - 1, data));
      new_panelStatus = (webString.substring(data - 1, data)).toInt();
    }

    if (new_panelStatus != panel_status) {
      panel_status = new_panelStatus;;

      if (panel_status == true) {
        lcd.clear();
        lcd.print("RUN");
        Serial.println("ENCENDIDO");
        digitalWrite(8, HIGH);
        digitalWrite(5, LOW);
        iotkit.send("status", panel_status);
      }
      else {
        lcd.clear();
        lcd.print("STOPPED");
        Serial.println("PARADO");
        digitalWrite(8, LOW);
        digitalWrite(5, HIGH);
        iotkit.send("status", panel_status);
      }
    }

    client.stop();
  }
}

void checkParams() {
  int a = analogRead(pinTempSensor);

  float R = 1023.0 / ((float)a) - 1.0;
  R = 100000.0 * R;

  float temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15; //convert to temperature via datasheet ;
  //Serial.println(temperature);

  if ((temperature > 31) && (alarma == false)) {
    alarma = true;
    lcd.clear();
    lcd.print("ALARM!!");
    digitalWrite(8, LOW);
    digitalWrite(5, HIGH);
    iotkit.send("status", panel_status);
    digitalWrite(7, HIGH);
    delay(250);
    digitalWrite(7, LOW);
    delay(250);
    digitalWrite(7, HIGH);
    delay(250);
    digitalWrite(7, LOW);
    delay(250);
    digitalWrite(7, HIGH);
    delay(250);
    digitalWrite(7, LOW);
  }
  if ((temperature < 28) && (alarma == true)) {
    alarma = false;
    lcd.clear();
    lcd.print("RUN");
    digitalWrite(8, HIGH);
    digitalWrite(5, LOW);
    iotkit.send("status", panel_status);
    delay(1000);
  }


}

