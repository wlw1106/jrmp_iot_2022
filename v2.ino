#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define echoPin 12 // attach pin D6 Arduino to pin Echo of HC-SR04
#define trigPin 13 //attach pin D7 Arduino to pin Trig of HC-SR04 
LiquidCrystal_I2C lcd(0x27, 20, 4);

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
bool setItOnce = false;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  lcd.print("Connecting Network -Lik Wai iPhone");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  client.setServer(ThingsboardHost, 1883);
  amount = 0;
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(Pin_D1, OUTPUT);
  pinMode(Pin_D2, OUTPUT);
  lcd.clear();
  lcd.print("Counting...");
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
  Serial.print(analogRead(0));
  F = analogRead(0) - 200;
  delay(200);
  Serial.print(" | ");
  digitalWrite(Pin_D1, LOW); //  Turn D1 Off
  digitalWrite(Pin_D2, HIGH); // Turn D2 On
  Serial.print(analogRead(0));
  B = analogRead(0) - 200;
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
  if (!client.connected()) {
    reconnect();
  }
  readD1 = analogRead1();
  delay(200);
  readD2 = analogRead2();
  delay(200);
  if (readD2 < B && readD1 < F) {
    delay(500);
    lcd.clear();
    lcd.print("Please DO NOT block the corridor!!");
    delay(2000);
    lcd.clear();
    lcd.print(amount);
    delay(1000);
    return;
  } else {
    if (readD2 < B) { // A0 300 A1 100
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      if (!setItOnce) {
        distance2 = duration * 0.034 / 2;
        setItOnce = true;
      }
      Serial.print("1. F: ");
      Serial.print(readD1);
      Serial.print(" | B: ");
      Serial.println(readD2);
      Serial.print("Distance2: ");
      Serial.print(distance2);
      Serial.println(" cm");
      digitalWrite(Pin_D1, HIGH);
      digitalWrite(Pin_D2, LOW);
      delay(100);
      if (analogRead(0) < F) { // A0 100
        distance3 = duration * 0.034 / 2;
        Serial.print("2. F: ");
        Serial.print(analogRead1());
        Serial.print(" | B: ");
        Serial.println(analogRead2());
        Serial.print("Distance3: ");
        Serial.print(distance3);
        Serial.println(" cm");
        if (distance3 > distance2) {
        lcd.clear();
        amount += 1;
        lcd.print(amount);
        getAndSendData();
        Serial.println(amount);
        delay(1000);
        setItOnce = false;
        return;
        }
      }
    }
    if (readD1 < F) { // A0 100 A1 300
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      duration = pulseIn(echoPin, HIGH);
      if (!setItOnce) {
        distance2 = duration * 0.034 / 2;
        setItOnce = true;
      }
      Serial.print("3. F: ");
      Serial.print(readD1);
      Serial.print(" | B: ");
      Serial.println(readD2);
      Serial.print("Distance2: ");
      Serial.print(distance2);
      Serial.println(" cm");
      digitalWrite(Pin_D1, LOW);
      digitalWrite(Pin_D2, HIGH);
      delay(100);
      if (analogRead(0) < B) { // A1 100
        distance3 = duration * 0.034 / 2;
        Serial.print("4. F: ");
        Serial.print(analogRead1());
        Serial.print(" | B: ");
        Serial.println(analogRead2());
        Serial.print("Distance3: ");
        Serial.print(distance3);
        Serial.println(" cm");
        if (distance2 < distance3) {
        lcd.clear();
        amount -= 1;
        lcd.print(amount);
        getAndSendData();
        Serial.println(amount);
        delay(1000);
        setItOnce = false;
        return;
        }
      }
    }
  }
}

void getAndSendData() { // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Number\":";
  payload += amount;
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
    }
    Serial.print("Connecting to ThingsBoard node ..."); // Attempt to connect (clientId, username, password)
    if (client.connect("Esp8266", TOKEN, NULL)) {
      Serial.println("[DONE]");
    } else {
      Serial.print("[FAILED] [ rc = ");
      Serial.println(" : retrying in 5 seconds]");
      delay(500);
    }
  }
}
