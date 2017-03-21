/*
    Carriots.com
    Created 28 Mar 2017

    This sketch sends streams to Carriots using the MQTT service.
*/
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h> // You must include the library here in order to use it in the sketch

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xBD};
byte ip[] = {10, 22, 72, 31};
byte DNS[] = {8, 8, 8, 8};
byte gateway[] = {10, 22, 72, 1};
byte subnet[] = {255, 255, 255, 0};

////////////////////////////////////////
// CARRIOTS Connectivity definitions. //
////////////////////////////////////////
#define CARRIOTS_DEVICE       "MiCasa-Temperatura"
#define CARRIOTS_USERNAME     "aprendiendoarduino"
#define CARRIOTS_APIKEY       "APIKEY"

#define CARRIOTS_MQTT_PORT    1883
#define CARRIOTS_MQTT_HOST    "mqtt.carriots.com"
#define CARRIOTS_MQTT_TOPPIC  "/streams" // Or /status

const String DEVICE_ID = CARRIOTS_DEVICE "@" CARRIOTS_USERNAME "." CARRIOTS_USERNAME;
////////////////////////////////////////

// Initialize the Ethernet client.
EthernetClient eth_client;
// Instantiate an instance of the MQTT client.
PubSubClient mqtt_client(CARRIOTS_MQTT_HOST, CARRIOTS_MQTT_PORT, eth_client);

void mqtt_connect() {
  // Loop until we're connected to MQTT.
  while (!mqtt_client.connected()) {
    Serial.print("Attempting MQTT connection... ");
    // Attempt to connect.
    if (mqtt_client.connect(CARRIOTS_USERNAME, CARRIOTS_APIKEY, NULL)) {
      Serial.println("Success");
      // Failed connection.
    } else {
      Serial.print("Failed connection: rc=");
      Serial.print(mqtt_client.state());
      Serial.println(". Retry on 1 second.");
      // Wait 1 second before retrying
      delay(1000);
    }
  }
}

void mqtt_publish(char* frame) {
  // Note - the default maximum packet size is 128 bytes.
  // You may need to increase the value of MQTT_MAX_PACKET_SIZE in
  //   Arduino/libraries/pubsubclient/src/PubSubClient.h
  //   (Increased to 256 on this example)
  Serial.print("Publishing at " CARRIOTS_MQTT_TOPPIC ": ");
  Serial.println(frame);
  // Publish data frame.
  if (mqtt_client.publish(CARRIOTS_APIKEY CARRIOTS_MQTT_TOPPIC, frame)) { // No comma between APIKEY and TOPPIC.
    Serial.println("Published.");
  } else {
    Serial.println("Failed publishing.");
  }
  Serial.println();
}

char* get_frame() {
  // Build Data JSON.
  String data_json  = "{\"Sample_Value_1\":10,\"Sample_Value_2\":\"Sample 2\"}";
  // Build Frame JSON.
  String frame_json = "{\"protocol\":\"v2\",\"device\":\"" + DEVICE_ID + "\",\"at\":1488447613,\"data\":" + data_json + "}";
  // Dump to "char array" and return.
  //   (mqtt.publish method does not works with "String" type, so it must be "casted" to "char array")
  int frame_length = frame_json.length() + data_json.length() + 1;
  char frame_buffer[frame_length];
  frame_json.toCharArray(frame_buffer, frame_length);
  return frame_buffer;
}

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);

  // Start the Ethernet connection:
  Ethernet.begin(mac, ip, DNS, gateway, subnet);         // Start the Ethernet connection
  delay(1000);                                // Give the Ethernet shield a second to initialize
}

void loop() {
  // Connect to MQTT.
  if (!mqtt_client.connected()) {
    mqtt_connect();
  }
  // Send frame.
  mqtt_publish(get_frame());
  delay(5000);
}
