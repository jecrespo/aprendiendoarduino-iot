#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include <aJSON.h>
#include <stdio.h>
#include "Arduino.h"
#include "IotkManager.h"

#define SERVER "109.228.56.48"
#define APIKEY "Bearer eyJ0eXAiOiJKV1QiLCJhbGciOiJSUzI1NiJ9.eyJqdGkiOiIzMThmZjdhNS05YjVmLTRhMzAtYTcwZC1iMzk5NTlkOWZkMTYiLCJpc3MiOiJodHRwOi8vZW5hYmxlaW90LmNvbSIsInN1YiI6ImYzMjg1ZjY3LWIzZjQtNDQyNy1hMDVmLTg3NjU3MWJkOWYzNyIsImV4cCI6IjIwMTctMDMtMjdUMTY6NDM6MjYuNDcwWiIsImFjY291bnRzIjp7IjA2NzFiNmMzLWY5NTctNDIzZS05NDlhLTMyNTQwZDNjNTg5ZCI6ImFkbWluIn19.dv6jXCbe6VpRjppLvH19XTnBkC3j_3SYMubNg6NHQmYlE037ic4ga6Lp-oFN2L3OJeHs4f5k-h-Pcs0dQ4g_Tv6Jo8O7lDZXWdZYHTE5hyGmVoLLDIXS5xuoqZx66ZEbaPQYUjmu_K1w7AT8y7SUrLXmsp-vgwr0NP6JIjzP1PQ"

IotkManager::IotkManager()
{
  EthernetClient client;
}

//Check if generates
bool IotkManager::retriveData(String master_device, String node_key) {
  if (client.connect(SERVER, 80)) {   // If theres a successful connection
    String json = "{\"from\": -10,\"targetFilter\": {\"deviceList\": [\""+master_device+"\"]},\"metrics\": [{\"id\": \""+node_key+"\",\"op\": \"none\"}]}";
    client.println("POST /v1/api/accounts/0671b6c3-f957-423e-949a-32540d3c589d/data/search HTTP/1.1");
    client.println("Host: 109.228.56.48");
    client.print("Authorization: ");
    client.println (APIKEY);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println("Connection: close");
    client.println();
    client.println(json);
  }
  delay (1000);
  String webString = "";
  if (client.available()) {
    while (client.available()) {
      char c = client.read();
      webString += c;
    }
    client.stop();
    if (webString.endsWith("\"points\":[]}]}")){
      return(0);
    }
    else {
      int data = webString.lastIndexOf("\"}]}]}");
      if(webString.substring(data-1,data) == "1")return(1); else return(0);
    }
  }
}

void IotkManager::initIotk(IoTkit iotkit, String device_id)
{
	Serial.begin(9600);
    // Check connectivity
    system("iotkit-admin test > /dev/ttyGS0 2>&1");
    // Replace YOUR_DEVICE_ID with your value
    //const char* command = 'iotkit-admin set-device-id ' + device_id + ' > /dev/ttyGS0 2>&1';
    String command = "iotkit-admin set-device-id " + device_id + " > /dev/ttyGS0 2>&1";
    system((const char*)command[0]);
     // After activation we should restart iotkit-agent service
    // If your installation is local, execute scripts start-agent.sh and stop-agent.sh instead providing full path to them
    system("systemctl stop iotkit-agent > /dev/ttyGS0 2>&1");
    delay(1000);
    system("systemctl start iotkit-agent > /dev/ttyGS0 2>&1");
    for(int i = 0; i < 3; i++) {
      delay(1000);
      system("systemctl status iotkit-agent > /dev/ttyGS0 2>&1");
    }   
    iotkit.begin();
}
