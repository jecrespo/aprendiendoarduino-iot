#ifndef LcdEmitter_h
#define LcdEmitter_h
#include "Arduino.h"
#include "rgb_lcd.h"
#include "LcdEmitter.h"

class LcdEmitter
{
  public:
	  LcdEmitter(String emitterName, String emitterType, float refreshTime);
    String getComponentName();
	  float getRefreshTime();
	  void printDisplay(String data, int y);
	  void cleanDisplay();
  private:
	  float _refreshTime;
	  String _name;
    String _type;
    rgb_lcd _lcd;
};

#endif

