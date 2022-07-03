#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "########"; // your wifi SSID
const char* password = "########"; // your wifi password

#define TOKEN "0ltoOWWpxAltNsyXWQpA" // thinksboard TOKEN (https://demo.thingsboard.io/dashboard/31615a60-f9c5-11ec-b8bd-8161cb0d156d?publicId=02100d50-f9c6-11ec-b8bd-8161cb0d156d)

int number;

char ThingsboardHost[] = "demo.thingsboard.io";
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;

void setup() {
  pinMode(5, INPUT); //+
  pinMode(4, INPUT); //-
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  } Serial.println("");
  Serial.print("connected to");
  Serial.println(ssid);
  client.setServer(ThingsboardHost, 1883);
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }
  if (digitalRead(5) == LOW) {
    number += 1;
    getAndSendData();
    delay(500);
  } else if (digitalRead(4) == LOW) {
    number -= 1;
    getAndSendData();
    delay(500);
  }
}

void getAndSendData() { // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Number\":";
  payload += number;
  /*payload += ",";
  payload += "\"Temperature\":";
  payload += 25;*/
  payload += "}";
  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish("v1/devices/me/telemetry", attributes);
  Serial.println(attributes);
}

void reconnect() { // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      } Serial.println("Connected to AP");
    } Serial.print("Connecting to ThingsBoard node ..."); // Attempt to connect (clientId, username, password)
    if ( client.connect("Esp8266", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.println( " : retrying in 5 seconds]" );
      delay(500);
    }
  }
}
