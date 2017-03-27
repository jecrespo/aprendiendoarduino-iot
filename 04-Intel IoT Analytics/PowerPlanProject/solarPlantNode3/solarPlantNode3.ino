#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include <aJSON.h>
#include <stdio.h>

#include "Arduino.h"
#include "Sensor.h"
#include "TempSensor.h"
#include "LightSensor.h"
#include "ButtonSensor.h"
#include "LedEmitter.h"
#include "IotkManager.h"
#include "LcdEmitter.h"
#include "PotenciometerSensor.h"

IotkManager iotkManager;
IoTkit iotkit;  

//Configure
String nodoName = "nodo_3";
String master_device = "AA:AA:AA:AA:AA:01";
String nodeKey = "2a3f7d55-1a2d-4b58-a4a6-8f8058757643";
String deviceId = "AA:BB:CC:DD:EE:FF";
//End configure

int refreshTime = 1500;
LightSensor lightS(A0,"radiation", "radiation.v1.0", refreshTime);
TempSensor tempS(A1,"temperature", "temperature.v1.0" ,refreshTime);
ButtonSensor buttonS(4,"button", "powerswitch.v1.0", refreshTime);
LedEmitter ledBlue(3,"led_blue","ledemitter.v.1.0", refreshTime);
LedEmitter ledRed(7,"led_red","ledemitter.v.1.0", refreshTime);
LcdEmitter lcdEmitter("lcd_emitter", "lcdemitter.v.1.0", refreshTime);
PotenciometerSensor potenciometerSensor(2,"power_demand","powerdemand.v.1.0",refreshTime);

//Config
const int highTemp = 27;
bool manualStop = false;
bool master = false;

void setup() {
  
  if (master_device == deviceId) {
    master = true;
  }
  
  //LCD Setting up
  lcdEmitter.printDisplay("Starting...",0);
  iotkManager.initIotk(iotkit, deviceId);
  ledRed.setupInit();
  ledBlue.setupInit();
  tempS.registerIotkComponent(iotkit);
  lightS.registerIotkComponent(iotkit);
  buttonS.registerIotkComponent(iotkit);
  
  if(master){
    potenciometerSensor.registerIotkComponent(iotkit);
  }
}

void loop() {  
  int demand;
  bool generateEnergy = iotkManager.retriveData(master_device,nodeKey);
  
  //Metrics
  if(generateEnergy){
    tempS.sendMetric(iotkit);
    lightS.sendMetric(iotkit);
    buttonS.sendMetric(iotkit);
  }
  
  //Master
  if (master)
  {
    potenciometerSensor.sendMetric(iotkit);
    demand = potenciometerSensor.getValue();
    
    //Setting up nodes
    if (demand == 0)
    {
      iotkit.send("nodo1",0);
      iotkit.send("nodo2",0);
      iotkit.send("nodo3",0);
      iotkit.send("nodo4",0);
    }    
    else if (demand <= 25)
    {
      iotkit.send("nodo1",1);
      iotkit.send("nodo2",0);
      iotkit.send("nodo3",0);
      iotkit.send("nodo4",0);
    }
    else if (demand <= 50)
    {
      iotkit.send("nodo1",1);
      iotkit.send("nodo2",1);
      iotkit.send("nodo3",0);
      iotkit.send("nodo4",0);
    }
    else if (demand <= 75)
    {
      iotkit.send("nodo1",1);
      iotkit.send("nodo2",1);
      iotkit.send("nodo3",1);
      iotkit.send("nodo4",0);
    }
    else
    {
      iotkit.send("nodo1",1);
      iotkit.send("nodo2",1);
      iotkit.send("nodo3",1);
      iotkit.send("nodo4",1);
    }
  }

  //Generate
  bool tempAlarm = false;
  
  if(tempS.getValue() > highTemp){
    tempAlarm = true;
  }
  
  int power = lightS.getValue();
  manualStop = buttonS.getValue();
  
  //Powering
  if ((not generateEnergy) or manualStop or tempAlarm)
  {
    power = 0;
  }
  
  //Print lights
  if(power > 20)
  {
	  ledBlue.setHigh();
	  ledRed.setLow();
  }
  else {
	  ledRed.setHigh();
	  ledBlue.setLow();
  }
  
  //Print LCD
  lcdEmitter.cleanDisplay();
  if(manualStop){
    lcdEmitter.printDisplay("Manual stop", 0);
  }else if(tempAlarm){
    lcdEmitter.printDisplay("Temp alarm", 0);
    lcdEmitter.printDisplay("Node disabled", 1);
  }else if(generateEnergy){
    lcdEmitter.printDisplay((String)("Power: "+(String)power+"%"), 0);  
    lcdEmitter.printDisplay("Temp: "+((String)((int)tempS.getValue())+"C"), 1);
  }else{
    lcdEmitter.printDisplay("Node disabled", 1);
  }
}


