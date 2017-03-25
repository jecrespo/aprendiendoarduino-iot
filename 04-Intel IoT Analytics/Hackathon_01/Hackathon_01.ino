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

const int TouchPin = 6;
const int ledPin = 5;

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k
const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5

Timer t;
IoTkit iotkit;

boolean lastread;

void setup()
{
  Serial.begin(115200);
  iotkit.begin();
  delay(10500);
  pinMode(TouchPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(7, OUTPUT);
  lastread = digitalRead(TouchPin);
  t.every(15000, sendTemp);
}

void loop()
{
  t.update();

  iotkit.receive(callback);

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
    iotkit.send("freno", sensorValue);
    lastread = sensorValue;
  }
}

void callback(char* json) {
  Serial.println(json);
}

void sendTemp()
{
  int a = analogRead(pinTempSensor);

  float R = 1023.0 / ((float)a) - 1.0;
  R = 100000.0 * R;

  float temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15) - 273.15; //convert to temperature via datasheet ;

  Serial.print("temperature = ");
  Serial.println(temperature);

  iotkit.send("temperatura", temperature);

  int sensorValue = analogRead(1); 
  float Rsensor=(float)(1023-sensorValue)*10/sensorValue;

  iotkit.send("distancia", Rsensor);

}

