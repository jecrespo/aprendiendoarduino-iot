#include <SPI.h>
#include <Ethernet.h>
#include "Timer.h"
#include <PubSubClient.h> //Para MQTT

#define OPEN 1
#define CLOSE 0

#define CARRIOTS_MQTT_PORT 1883
#define CARRIOTS_MQTT_HOST "mqtt.carriots.com"
#define CARRIOTS_MQTT_TOPIC "/streams" // Or /status

#define APIKEY "APIKEY" // Replace with your Carriots apikey
#define DEVICE "Arduino99" // Replace with the id_developer of your device
#define USERNAME "aprendiendoarduino"

const String DEVICE_ID = DEVICE "@" USERNAME "." USERNAME;

byte mac[] = {0x90, 0xA2, 0xDA, 0x0F, 0x70, 0xYY};  //Sustituir YY por el numero de MAC correcto

IPAddress server(82, 223, 244, 60); // api.carriots.com IP Address

EthernetClient client; // Cliente API REST
EthernetClient client2; // Cliente MQTT

Timer t;

int door = CLOSE;       // Current status
int newdoor = CLOSE;    // New status

PubSubClient MQTTclient(CARRIOTS_MQTT_HOST, CARRIOTS_MQTT_PORT, client2);


// The setup routine runs once when you press reset
void setup() {

  Serial.begin(9600);             // Start serial port
  Serial.println(F("Starting"));

  t.every(10000, sendDataMQTT); //change between sendDataMQTT and sendData depending on the senddata method: MQTT or API REST

  pinMode(6, INPUT_PULLUP);	//Puerta
  door = digitalRead(6);
  newdoor = door;

  Serial.print("Topic de publicacion: ");
  Serial.println(CARRIOTS_MQTT_TOPIC);

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for (;;)
      ;
  }
  else {
    Serial.print("IP asignada por DHCP: ");
    Serial.println(Ethernet.localIP());
  }
  delay(1000);                    						// Give the Ethernet shield a second to initialize
}

// The loop routine runs over and over again forever
void loop() {
  t.update();

  newdoor = digitalRead(6);

  if (newdoor != door) {        // Check if we have a change in status
    Serial.println(F("Send Stream"));
    door = newdoor;             // Status update and send stream
    sendStatus();
  }

  // Connect to MQTT.
  if (!MQTTclient.connected()) {
    mqtt_connect();
  }
}

// Send stream to Carriots
void sendStatus() {

  String txt = "";          // Text to send
  if ( door == CLOSE ) {   // Alarm OFF
    txt = "CLOSE";
  } else {                  // Alarm ON
    txt = "OPEN";
  }
  Serial.println(txt);      // For debugging purpose only

  if (client.connect(server, 80)) {   // If theres a successful connection
    Serial.println(F("connected"));
    // Build the data field
    String json = "{\"protocol\":\"v2\",\"device\":\"" + DEVICE_ID + "\",\"at\":\"now\",\"data\":{\"door\":\"" + txt + "\"}}";
    Serial.println(json);      // For debugging purpose only

    // Make a HTTP request
    client.println("POST /status HTTP/1.1");
    client.println("Host: api.carriots.com");
    client.println("Accept: application/json");
    client.println("User-Agent: Arduino-Carriots");
    client.println("Content-Type: application/json");
    client.print("carriots.apikey: ");
    client.println(APIKEY);
    client.print("Content-Length: ");
    int thisLength = json.length();
    client.println(thisLength);
    client.println("Connection: close");
    client.println();

    client.println(json);
  }
  else {
    // If you didnt get a connection to the server:
    Serial.println(F("connection failed"));
  }

  delay (1000);
  String webString = "";
  if (client.available()) {
    Serial.println("Respuesta del Servidor---->");
    while (client.available()) {
      char c = client.read();
      webString += c;
    }
    Serial.println(webString);

    client.stop();
  }
}

void sendDataMQTT() {
  // Note - the default maximum packet size is 128 bytes.
  // You may need to increase the value of MQTT_MAX_PACKET_SIZE in
  // Arduino/libraries/pubsubclient/src/PubSubClient.h
  // Based in https://www.carriots.com/tutorials/arduino_carriots/arduino_mqtt

  int pot = analogRead(A0);
  Serial.print("Valor Potenciometro: ");
  Serial.println(pot);

  String json = "{\"protocol\":\"v2\",\"device\":\"" + DEVICE_ID + "\",\"at\":\"now\",\"data\":{\"potenciometro\":\"" + (String)pot + "\"}}";
  const char *buff = json.c_str();

  Serial.print("Publishing at " CARRIOTS_MQTT_TOPIC ": ");
  Serial.println(json);

  // Publish data frame.
  if (MQTTclient.publish(APIKEY CARRIOTS_MQTT_TOPIC, buff)) { // No comma between APIKEY and TOPIC
    Serial.println("Published.");
  } else {
    Serial.println("Failed publishing.");
  }
  Serial.println();
}

void mqtt_connect() {
  // Loop until we're connected to MQTT.
  while (!MQTTclient.connected()) {
    Serial.print("Attempting MQTT connection... ");
    // Attempt to connect.
    if (MQTTclient.connect(USERNAME, APIKEY, NULL)) {
      Serial.println("Success");
      // Failed connection.
    } else {
      Serial.print("Failed connection: rc=");
      Serial.print(MQTTclient.state());
      Serial.println(". Retry on 1 second.");
      // Wait 1 second before retrying
      delay(1000);
    }
  }
}
