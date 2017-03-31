#include <Ethernet.h>
#include <SPI.h>

#define PIN "0000"

byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0xB3, 0xBD};

IPAddress iotIP (10, 22, 72, 31);
IPAddress dnsIP (8, 8, 8, 8);

char url[] = "www.aprendiendoarduino.com";

EthernetClient client;
String webString = "";
String telefono = "600000000";
String mensaje = "MENSAJE_DE_ALERTA";

boolean estado_boton;

void setup()
{
  pinMode(6, INPUT_PULLUP);
  estado_boton = digitalRead(6);
  Serial.begin(9600);
  Serial.println("inicializando red...");
  Ethernet.begin(mac, iotIP, dnsIP);
  delay(1000); //give the ethernet a second to initialize
}

void loop()
{
  boolean nuevo_estado = digitalRead(6);

  if (nuevo_estado != estado_boton) {
    if (nuevo_estado == LOW) { //Flanco descendente
      mandaSMS();
    }
    estado_boton = nuevo_estado;
  }
  delay(250);
}

void mandaSMS() {
  Serial.println("enviando mensaje... ");

  Serial.println("connecting to server...");
  if (client.connect(url, 80)) {
    Serial.println("connected");
    client.print("GET /servicios/SMS/saveSMS.php?telefono=");
    client.print(telefono);
    client.print("&mensaje=");
    client.print(mensaje);
    client.print("&pin=");
    client.print((String)PIN);
    client.println(" HTTP/1.1");
    client.println("Host: www.aprendiendoarduino.com");
    client.println("Connection: close");
    client.println();
    Serial.print("GET /servicios/SMS/saveSMS.php?telefono=");
    Serial.print(telefono);
    Serial.print("&mensaje=");
    Serial.print(mensaje);
    Serial.print("&pin=");
    Serial.print((String)PIN);
    Serial.println(" HTTP/1.1");
    Serial.println("Host: www.aprendiendoarduino.com");
    Serial.println("Connection: close");
    Serial.println();

    delay(2000);

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
      Serial.println("Servidor no responde...");
      client.stop();
    }
  }
  else {
    Serial.println("connection failed");
  }
}
