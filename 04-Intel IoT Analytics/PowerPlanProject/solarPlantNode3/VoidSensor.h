#ifndef VoidSensor_h
#define VoidSensor_h

#include "Sensor.h"
#include "VoidSensor.h"

class VoidSensor : public Sensor
{
  public:
	VoidSensor(int pin, String sensorName, String sensorType, float refreshTime);
  float getValue();
	void setValue(float value);

  private:
    int _pin;
    String _name;
    float _refreshTime;
    String _type;
	  float _value;
};

#endif

