#include <Ethernet.h>
#include <SPI.h>
#include "Timer.h"

#define NUM_ARDUINO 99	//Sustituir X por el numero de Arduino correcto
#define KEY "ABCDEFGHI"

byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xBD};

IPAddress iotIP (10, 22, 72, 31);
IPAddress dnsIP (8, 8, 8, 8);

char url[] = "opendata.aemet.es";

EthernetClient client;
Timer t;
String webString = "";

void setup()
{

  Serial.begin(9600);
  Serial.println("inicializando red...");
  Ethernet.begin(mac, iotIP, dnsIP);
  delay(1000); //give the ethernet a second to initialize

  Serial.println("Iniciando datalogger AEMET...");
  delay(1000);
  t.every(5000, grabaDatos);
}

void loop()
{
  t.update();
}

void grabaDatos() {
  Serial.println("connecting to server...");
  if (client.connect(url, 80)) {
    Serial.println("connected");
    client.print("GET /opendata/api/observacion/convencional/datos/estacion/9170?api_key=" KEY " HTTP/1.1");
    client.println("Host: opendata.aemet.es");
    client.println();

    delay(6000);

    webString = "";
    if (client.available()) {
      Serial.println("Respuesta del Servidor---->");
      while (client.available()) {
        char c = client.read();
        webString += c;
      }
      Serial.println(webString);
      client.stop();
    }
    else {
      Serial.println("Servidor no responde");
      client.stop();
    }
  }
  else {
    Serial.println("connection failed");
  }
}
