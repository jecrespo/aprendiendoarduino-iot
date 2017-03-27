#ifndef LedEmitter_h
#define LedEmitter_h
#include "Arduino.h"
#include "LedEmitter.h"

class LedEmitter
{
  public:
	  LedEmitter(int pin, String emitterName, String emitterType, float refreshTime);
    String getComponentName();
	  float getRefreshTime();
    int getPin();
    void setupInit();
	  void setHigh();
	  void setLow();
  private:
    int _pin;
	  float _refreshTime;
	  String _name;
    String _type;
};

#endif

