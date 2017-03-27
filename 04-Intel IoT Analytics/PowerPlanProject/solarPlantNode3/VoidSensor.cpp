#include "Arduino.h"
#include "Sensor.h"
#include "math.h"
#include "VoidSensor.h"

VoidSensor::VoidSensor(int pin, String sensorName, String sensorType, float refreshTime) : Sensor(pin, sensorName, sensorType, refreshTime){}

float VoidSensor::getValue()
{
    return _value;
}

void VoidSensor::setValue(float val){
	_value = val;
}
