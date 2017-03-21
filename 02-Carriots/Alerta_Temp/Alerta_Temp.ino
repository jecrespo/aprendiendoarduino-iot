/*
    Sistema de registro de temperatura que manda alertas y sensor de proximidad que manda alerta cuando se abre una puerta.

	Manda datos via MQTT o API REST

	Para Arduino con Ethernet Shield

    This sketch sends streams to Carriots

*/
#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
#include "Timer.h"
#include <PubSubClient.h> //Para MQTT

#define DHT22_PIN 7
#define DHTTYPE DHT22
#define SESOR_PIN 3
#define OPEN 1
#define CLOSE 0

#define CARRIOTS_MQTT_PORT 1883
#define CARRIOTS_MQTT_HOST "mqtt.carriots.com"
#define CARRIOTS_MQTT_TOPIC "/streams" // Or /status

#define APIKEY "APIKEY" // Replace with your Carriots apikey
#define DEVICE "MiCasa-Temperatura" // Replace with the id_developer of your device
#define USERNAME "aprendiendoarduino"

const String DEVICE_ID = DEVICE "@" USERNAME "." USERNAME;

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xBD};
byte ip[] = {10, 22, 72, 31};
byte DNS[] = {8, 8, 8, 8};
byte gateway[] = {10, 22, 72, 1};
byte subnet[] = {255, 255, 255, 0};

IPAddress server(82, 223, 244, 60); // api.carriots.com IP Address

EthernetClient client; // Initialize the library instance
EthernetClient client2; // Initialize the library instance for MQTT client

Timer t;
DHT sondaT(DHT22_PIN, DHTTYPE);

int door = CLOSE;       // Current status
int newdoor = CLOSE;    // New status

PubSubClient MQTTclient(CARRIOTS_MQTT_HOST, CARRIOTS_MQTT_PORT, client2);


// The setup routine runs once when you press reset
void setup() {

  Serial.begin(9600);             // Start serial port
  Serial.println(F("Starting"));

  t.every(10000, sendDataMQTT); //change between sendDataMQTT and sendData depending on the senddata method: MQTT or API REST

  sondaT.begin();

  pinMode(SESOR_PIN, INPUT_PULLUP);
  door = digitalRead(SESOR_PIN);

  Serial.print("Topic de publicacion: ");
  Serial.println(APIKEY CARRIOTS_MQTT_TOPIC);

  Ethernet.begin(mac, ip, DNS, gateway, subnet);         // Start the Ethernet connection
  delay(1000);                    						// Give the Ethernet shield a second to initialize

}

// The loop routine runs over and over again forever
void loop() {
  t.update();

  newdoor = digitalRead(SESOR_PIN);

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

void sendData() {
  float hum = sondaT.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = sondaT.readTemperature();
  Serial.print("Temperatura Leida: ");
  Serial.println(temp);
  Serial.print("Humedad Leida: ");
  Serial.println(hum);

  if (client.connect(server, 80)) {   // If theres a successful connection
    Serial.println(F("connected"));
    // Build the data field
    String json = "{\"protocol\":\"v2\",\"device\":\"" + DEVICE_ID + "\",\"at\":\"now\",\"data\":{\"temperatura\":\"" + (String)temp + "\",\"humedad\":\"" + (String)hum + "\"}}";
    Serial.println(json);      // For debugging purpose only

    // Make a HTTP request
    client.println("POST /streams HTTP/1.1");
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

  float hum = sondaT.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = sondaT.readTemperature();
  Serial.print("Temperatura Leida: ");
  Serial.println(temp);
  Serial.print("Humedad Leida: ");
  Serial.println(hum);

  String json = "{\"protocol\":\"v2\",\"device\":\"" + DEVICE_ID + "\",\"at\":\"now\",\"data\":{\"temperatura\":\"" + (String)temp + "\",\"humedad\":\"" + (String)hum + "\"}}";
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
