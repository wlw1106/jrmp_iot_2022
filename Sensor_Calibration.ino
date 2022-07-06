#define echoPin 12 // attach pin D6 Arduino to pin Echo of HC-SR04
#define trigPin 13 //attach pin D7 Arduino to pin Trig of HC-SR04 

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Lik Wai的iPhone"; // your wifi SSID
const char* password = "00000000"; // your wifi password

#define TOKEN "0ltoOWWpxAltNsyXWQpA" // thinksboard TOKEN (https://demo.thingsboard.io/dashboard/31615a60-f9c5-11ec-b8bd-8161cb0d156d?publicId=02100d50-f9c6-11ec-b8bd-8161cb0d156d)

char ThingsboardHost[] = "demo.thingsboard.io";
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int status = WL_IDLE_STATUS;

long amount;
long F; // D3
long B; // D4
long duration;
int distance1;
int distance2;
int distance3;
int Pin_D1 = 0; // D3, F
int Pin_D2 = 2; // D4, B
int readD1;
int readD2;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  client.setServer(ThingsboardHost, 1883);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(Pin_D1, OUTPUT);
  pinMode(Pin_D2, OUTPUT);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance1 = duration * 0.034 / 2;

  digitalWrite(Pin_D1, HIGH); // Turn D1 On
  digitalWrite(Pin_D2, LOW); // Turn D2 Off
  delay(200);
  Serial.print("F: ");
  Serial.print(analogRead(0));
  F = analogRead(0);

  delay(200);
  Serial.print(" | B: ");
  digitalWrite(Pin_D1, LOW); //  Turn D1 Off
  digitalWrite(Pin_D2, HIGH); // Turn D2 On
  Serial.print(analogRead(0));
  B = analogRead(0);

  if (distance1 != 0) {
    Serial.print(" | Distance1: ");
    Serial.print(distance1);
    Serial.println(" cm");
  }
}

int analogRead1() {
  digitalWrite(Pin_D1, HIGH); // Turn D1 On
  digitalWrite(Pin_D2, LOW); // Turn D2 Off
  return analogRead(0);
}

int analogRead2() {
  digitalWrite(Pin_D1, LOW); //  Turn D1 On
  digitalWrite(Pin_D2, HIGH); // Turn D2 Off
  return analogRead(0);
}

void loop() {
  if ( !client.connected() ) {
    reconnect();
  }
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance2 = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  readD1 = analogRead1(); // Read Analog value of first sensor
  delay(200);
  readD2 = analogRead2(); // Read Analog value of second sensor
  delay(200);
  // Displays the distance on the Serial Monitor
  if (distance1 != distance2) {
    if (distance2 < 2000) {
      Serial.print("F: ");
      Serial.print(readD1);
      Serial.print(" | B: ");
      Serial.println(readD2);
      Serial.print(" | [Distance2]: ");
      Serial.print(distance2);
      Serial.println(" cm");
      getAndSendData();
    }
  } else if (readD1 < (F+50) && readD1 > 0) {
    Serial.print("[F]: ");
    Serial.print(readD1);
    Serial.print(" | B: ");
    Serial.println(readD2);
    Serial.print(" | Distance2: ");
    Serial.print(distance2);
    Serial.println(" cm");
    getAndSendData();
  } else if (readD2 < (B+50) && readD2 > 0) {
    Serial.print("F: ");
    Serial.print(readD1);
    Serial.print(" | [B]: ");
    Serial.println(readD2);
    Serial.print(" | Distance2: ");
    Serial.print(distance2);
    Serial.println(" cm");
    getAndSendData();
  }
}

void getAndSendData() { // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Front Sensor\":";
  payload += readD1;
  payload += ",";
  payload += "\"Back Sensor\":";
  payload += readD2;
  payload += ",";
  payload += "\"Ultrasonic Sensor\":";
  payload += distance2;
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
