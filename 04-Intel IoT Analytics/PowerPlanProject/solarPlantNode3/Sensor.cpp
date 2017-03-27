#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include <aJSON.h>
#include <stdio.h>
#include "Arduino.h"
#include "Sensor.h"

Sensor::Sensor(int pin, String sensorName, String sensorType, float refreshTime)
{
  _pin = pin;
  _refreshTime = refreshTime;
  _name = sensorName;
  _type = sensorType;
  pinMode(_pin, INPUT);
}

void Sensor::registerIotkComponent(IoTkit iotkit){
  String command = "{ \"n\": \""+_name+"\", \"t\": \""+_type+"\"}\n";
  Serial.println("Registering component "+_name);
  iotkit.send((char*)&command[0]);
}

void Sensor::sendMetric(IoTkit iotkit){
  Serial.print("Metric sent: "+_name+"= ");
  Serial.println(getValue());
  iotkit.send((const char*)&_name[0], getValue());
}

int Sensor::getPin()
{
  return _pin;
}

String Sensor::getComponentName()
{
	return _name;
}

float Sensor::getRefreshTime()
{
	return _refreshTime;
}
