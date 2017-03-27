#include "Arduino.h"
#include "Sensor.h"
#include "math.h"
#include "ButtonSensor.h"

ButtonSensor::ButtonSensor(int pin, String sensorName, String sensorType, float refreshTime) : Sensor(pin, sensorName, sensorType, refreshTime){}

float ButtonSensor::getValue()
{
    int sensValue = digitalRead(Sensor::getPin());
    return sensValue;
}

