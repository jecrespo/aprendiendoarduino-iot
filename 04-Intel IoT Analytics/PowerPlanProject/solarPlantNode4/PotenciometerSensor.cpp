#include "Arduino.h"
#include "Sensor.h"
#include "math.h"
#include "PotenciometerSensor.h"

PotenciometerSensor::PotenciometerSensor(int pin, String sensorName, String sensorType, float refreshTime) : Sensor(pin, sensorName, sensorType, refreshTime){}

float PotenciometerSensor::getValue()
{
    int sensValue = map(analogRead(_pin),0,1024,0,100);
    return sensValue;
}

