#include "Arduino.h"
#include "LedEmitter.h"
	
LedEmitter::LedEmitter(int pin, String emitterName, String emitterType, float refreshTime)
{
	_pin = pin;
	_refreshTime = refreshTime;
	_name = emitterName;
	_type = emitterType;
}

void LedEmitter::setupInit()
{
  pinMode(_pin,OUTPUT);
}
  
void LedEmitter::setHigh()
{
   digitalWrite(_pin,HIGH);
}

void LedEmitter::setLow()
{
  digitalWrite(_pin,LOW);
}

int LedEmitter::getPin()
{
	return _pin;
}

String LedEmitter::getComponentName()
{
	return _name;
}

float LedEmitter::getRefreshTime()
{
	return _refreshTime;
}
