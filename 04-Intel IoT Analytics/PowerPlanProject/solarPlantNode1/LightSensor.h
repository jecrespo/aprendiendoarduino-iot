#ifndef LightSensor_h
#define LightSensor_h

#include "Sensor.h"
#include "LightSensor.h"

class LightSensor : public Sensor
{
  public:
	  LightSensor(int pin, String sensorName, String sensorType, float refreshTime);
    float getValue();

  private:
    int _pin;
    String _name;
    float _refreshTime;
    String _type;
};

#endif

