/*
ESP-COAP Server
*/

#include <ESP8266WiFi.h>
#include <coap_server.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11

#define dht_dpin D3
DHT dht(dht_dpin, DHTTYPE); 


// CoAP server endpoint url callback
void callback_light(coapPacket &packet, IPAddress ip, int port, int obs);

coapServer coap;

//WiFi connection info
const char* ssid = "SHAIVAL";
const char* password = "1234567890";

// LED STATE
bool LEDSTATE;

// CoAP server endpoint URL
void callback_light(coapPacket *packet, IPAddress ip, int port, int obs) {
  
  // send response
  char p[packet -> payloadlen + 1];
  memcpy(p, packet -> payload, packet -> payloadlen);
  p[packet->payloadlen] = NULL;
  Serial.println(p);

  String message(p);

  if (message.equals("off")) {
    digitalWrite(D7, LOW);
    Serial.println("if loop");
  }
  else if (message.equals("on")) {
    digitalWrite(D7, HIGH);
    Serial.println("else loop");
  } 
  char *light = (digitalRead(D7) > 0)? ((char *) "on") :((char *) "off");
  
   //coap.sendResponse(packet, ip, port, light);
   if(obs == 1)
    coap.sendResponse(light);
   else
    coap.sendResponse(ip, port, light);
}

void callback_Temp(coapPacket *packet, IPAddress ip, int port, int obs) {
  float t = dht.readTemperature();
  String msg = "Temperature: ";
  msg = msg + String(t);
  msg = msg + " C";
  char message[58];
  msg.toCharArray(message,58);
  
  coap.sendResponse(ip, port, message);
}
void callback_Humid(coapPacket *packet, IPAddress ip, int port, int obs) {
  float h = dht.readHumidity();
  String msg = "Humidity: ";
  msg = msg + String(h);
  msg = msg + " %";
  char message[58];
  msg.toCharArray(message,58);
  
  coap.sendResponse(ip, port, message);
}
void setup() {
  yield();
  //serial begin
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println(" ");

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    yield();
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());

  // LED State
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);
  LEDSTATE = true;

  // add server url endpoints.
  // can add multiple endpoint urls.
  coap.server(callback_light, "light");
  coap.server(callback_Temp, "Temperature");
  coap.server(callback_Humid, "Humidity");
  coap.start(5683);
}

void loop() {
  coap.loop();
  delay(1000);
}
