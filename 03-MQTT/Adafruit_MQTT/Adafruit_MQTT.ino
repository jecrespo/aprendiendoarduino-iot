#include <Ethernet.h>
#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* Ethernet Client Setup *****************************/
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xBD};

IPAddress iotIP (10, 22, 72, 31);
IPAddress dnsIP (8, 8, 8, 8);

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "aprendiendoarduino"
#define AIO_KEY         "MYKEY"


/************ Global State (you don't need to change this!) ******************/

//Set up the ethernet client
EthernetClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }

int pot;
boolean led;

/****************************** Feeds ***************************************/

// Setup a feeds for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish potenciometro = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/node99-pot");
Adafruit_MQTT_Publish iluminacion_remoto = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/node5-led");

// Setup a feed called 'LED' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/node99-led");

/*************************** Sketch Code ************************************/

void setup() {
  Serial.begin(115200);

  Serial.println(F("Adafruit MQTT demo"));

  // Initialise the Client
  Serial.print(F("\nInit the Client..."));
  Ethernet.begin(mac, iotIP, dnsIP);
  delay(1000); //give the ethernet a second to initialize

  mqtt.subscribe(&onoffbutton);

  pinMode(8, OUTPUT);
}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {  //mqtt.readSubscription(timeInMilliseconds) will sit and listen for up to 'time' for a message.
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);

      String lastread = (char *)onoffbutton.lastread;

      Serial.println(lastread);

      if (lastread == "OFF")
        digitalWrite(8, LOW);
      else
        digitalWrite(8, HIGH);
    }
  }

  pot = analogRead(A0);

  if (!potenciometro.publish(pot)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }

  boolean boton = readDigital(6);

  if (!iluminacion_remoto.publish(boton)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }

  // ping the server to keep the mqtt connection alive
  if (! mqtt.ping()) {  //keep alive la conexión con el MQTT server
    mqtt.disconnect();
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
