#ifndef TempSensor_h
#define TempSensor_h

#include "Sensor.h"
#include "TempSensor.h"

class TempSensor : public Sensor
{
  public:
	  TempSensor(int pin, String sensorName, String sensorType, float refreshTime);
    float getValue();

  private:
    int _pin;
    String _name;
    float _refreshTime;
    String _type;
};

#endif

