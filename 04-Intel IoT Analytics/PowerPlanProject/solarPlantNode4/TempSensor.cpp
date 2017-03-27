#include "Arduino.h"
#include "Sensor.h"
#include "math.h"
#include "TempSensor.h"

TempSensor::TempSensor(int pin, String sensorName, String sensorType, float refreshTime) : Sensor(pin, sensorName, sensorType, refreshTime){}

float TempSensor::getValue()
{
	int B=4275;                 // B value of the thermistor
    int R0 = 100000;            // R0 = 100k
    int a = analogRead(Sensor::getPin());
    
    float R = 1023.0/((float)a)-1.0;
    R = 100000.0*R;

    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet ;
    return temperature;
}

