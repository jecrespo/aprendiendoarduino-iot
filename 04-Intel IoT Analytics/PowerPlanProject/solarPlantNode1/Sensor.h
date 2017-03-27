#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include <aJSON.h>
#include <stdio.h>
#ifndef Sensor_h
#define Sensor_h
#include "Arduino.h"
#include "Sensor.h"

class Sensor
{
  public:
	  Sensor(int pin, String sensorName, String sensorType, float refreshTime);
    virtual float getValue() = 0;
	  void registerIotkComponent(IoTkit iotkit);
    void sendMetric(IoTkit iotkit);
	  String getComponentName();
	  float getRefreshTime();
    int getPin();
	
  private:
    int _pin;
	  float _refreshTime;
	  String _name;
    String _type;
};

#endif

