#ifndef PotenciometerSensor_h
#define PotenciometerSensor_h

#include "Sensor.h"
#include "PotenciometerSensor.h"

class PotenciometerSensor : public Sensor
{
  public:
	PotenciometerSensor(int pin, String sensorName, String sensorType, float refreshTime);
    float getValue();

  private:
    int _pin;
    String _name;
    float _refreshTime;
    String _type;
};

#endif

