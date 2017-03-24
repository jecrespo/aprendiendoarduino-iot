/***************************************************
  Adafruit MQTT Library Ethernet Example

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Alec Moore
  Derived from the code written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <Ethernet.h>
#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "dht.h"

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

#define DHT22_PIN 7
dht sondaT;
float temp;
float hum;

/****************************** Feeds ***************************************/

// Setup a feeds for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish temperatura = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/temperatura");
Adafruit_MQTT_Publish humedad = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/humedad");

// Setup a feed called 'LED' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED");

/*************************** Sketch Code ************************************/

void setup() {
  Serial.begin(115200);

  Serial.println(F("Adafruit MQTT demo"));

  // Initialise the Client
  Serial.print(F("\nInit the Client..."));
  Ethernet.begin(mac, iotIP, dnsIP);
  delay(1000); //give the ethernet a second to initialize

  mqtt.subscribe(&onoffbutton);
}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  int chk = sondaT.read22(DHT22_PIN);
  if (chk == DHTLIB_OK) {
    temp = sondaT.temperature;
    hum = sondaT.humidity;
    Serial.print("Temperatura Leida: ");
    Serial.println(temp);
    Serial.print("Humedad Leida: ");
    Serial.println(hum);
  }
  else {
    Serial.print("Error lectura sonda: ");
    Serial.println(chk);
  }

  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &onoffbutton) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoffbutton.lastread);
    }
  }

  if (chk == DHTLIB_OK) {
    // Now we can publish stuff!
    Serial.print(F("\nSending temperature val "));
    Serial.print(temp);
    Serial.print("...");
    if (! temperatura.publish(temp)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }

    Serial.print(F("\nSending humidity val "));
    Serial.print(hum);
    Serial.print("...");
    if (! humedad.publish(hum)) {
      Serial.println(F("Failed"));
    } else {
      Serial.println(F("OK!"));
    }
  }

  // ping the server to keep the mqtt connection alive
  if (! mqtt.ping()) {
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
