#ifndef ButtonSensor_h
#define ButtonSensor_h

#include "Sensor.h"
#include "ButtonSensor.h"

class ButtonSensor : public Sensor
{
  public:
	  ButtonSensor(int pin, String sensorName, String sensorType, float refreshTime);
    float getValue();

  private:
    int _pin;
    String _name;
    float _refreshTime;
    String _type;
};

#endif

