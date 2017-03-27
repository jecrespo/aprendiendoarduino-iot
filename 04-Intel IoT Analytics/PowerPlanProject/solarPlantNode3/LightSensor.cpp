#include "Arduino.h"
#include "Sensor.h"
#include "math.h"
#include "LightSensor.h"

LightSensor::LightSensor(int pin, String sensorName, String sensorType, float refreshTime) : Sensor(pin, sensorName, sensorType, refreshTime){}

float LightSensor::getValue()
{
    int sensValue = analogRead(Sensor::getPin());
    //min = 100, //max = 800
    return sensValue;
}

