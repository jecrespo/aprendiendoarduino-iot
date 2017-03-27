#include "Arduino.h"
#include "rgb_lcd.h"
#include "LcdEmitter.h"

LcdEmitter::LcdEmitter(String emitterName, String emitterType, float refreshTime)
{
	_refreshTime = refreshTime;
	_name = emitterName;
	_type = emitterType;
  rgb_lcd _lcd;
	_lcd.begin(16,2);
	_lcd.setRGB(0,174,239);
}

void LcdEmitter::printDisplay(String data, int y){
	_lcd.setCursor(0,y);
	_lcd.print(data);
}

void LcdEmitter::cleanDisplay(){
	_lcd.setCursor(0,0);
	_lcd.print("                ");
  _lcd.setCursor(0,1);
  _lcd.print("                ");
}

String LcdEmitter::getComponentName()
{
	return _name;
}

float LcdEmitter::getRefreshTime()
{
	return _refreshTime;
}
