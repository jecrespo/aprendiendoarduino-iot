#include <Ethernet.h>
#include <SPI.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* Ethernet Client Setup *****************************/
byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x70, 0xYY};	//Sustituir YY por el numero de MAC correcto

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

/****************************** Feeds ***************************************/

// Setup a feeds for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish potenciometro = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/node99-pot");
Adafruit_MQTT_Publish led_remoto = Adafruit_MQTT_Publish(&mqtt,  AIO_USERNAME "/feeds/node98-led");

// Setup a feed called 'LED' for subscribing to changes.
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/node99-led");

/*************************** Sketch Code ************************************/

void setup() {
  Serial.begin(115200);

  Serial.println(F("Adafruit MQTT demo"));

  // Initialise the Client
  Serial.print(F("\nInit the Client..."));
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for (;;)
      ;
  }
  else {
    Serial.print("IP asignada por DHCP: ");
    Serial.println(Ethernet.localIP());
  }
  delay(1000); //give the ethernet a second to initialize

  mqtt.subscribe(&onoffbutton);

  pinMode(8, OUTPUT);
  pinMode(6, INPUT_PULLUP);
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

  //Redad pot value
  uint32_t poten = analogRead(A0);

  Serial.print(F("\nSending pot val "));
  Serial.print(poten);
  Serial.print("...");

  if (!potenciometro.publish(poten)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

  //read button value
  boolean boton = digitalRead(6);
  char* estado_boton;
  if (boton) estado_boton = "ON";
  else estado_boton = "OFF";

  Serial.print(F("\nSending button val "));
  Serial.print(estado_boton);
  Serial.print("...");
  if (!led_remoto.publish(estado_boton)) {
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
