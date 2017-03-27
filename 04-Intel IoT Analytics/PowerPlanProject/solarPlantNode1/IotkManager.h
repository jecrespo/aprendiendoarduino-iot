#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include <aJSON.h>
#include <stdio.h>

#ifndef IotkManager_h
#define IotkManager_h
#include "Arduino.h"
#include "IotkManager.h"

class IotkManager
{
  public:
	  IotkManager();
    bool retriveData(String master_device, String node_key);
	  void initIotk(IoTkit iotkit, String device_id);
  private:
    EthernetClient client;
};

#endif

