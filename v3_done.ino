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
int runXTimes = 0;
String record_d = "";

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
  check_distance();
  distance1 = duration * 0.034 / 2;
  if (distance1 != 0) {
    Serial.print("Distance1: ");
    Serial.print(distance1);
    Serial.println(" cm");
  }
}

int analogRead1() { // F
  digitalWrite(Pin_D1, HIGH); // Turn D1 On
  digitalWrite(Pin_D2, LOW); // Turn D2 Off
  return analogRead(0);
}

int analogRead2() { // B
  digitalWrite(Pin_D1, LOW); //  Turn D1 On
  digitalWrite(Pin_D2, HIGH); // Turn D2 Off
  return analogRead(0);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if (runXTimes < 1) {
    delay(1000);
    F = analogRead1() - 50;
    delay(200);
    B = analogRead2() - 50;
    delay(200);
    if ((B - F) > 100) {
      F = analogRead1() - 50;
      delay(200);
    }
    if ((F - B) > 100) {
      B = analogRead2() - 50;
      delay(200);
    }
    Serial.print(F);
    Serial.print(" | ");
    Serial.println(B);
    runXTimes++;
  }

  readD1 = analogRead1();
  delay(100);
  readD2 = analogRead2();
  delay(100);

  if ((B - readD2) > 200) {
    F = analogRead1() - 80;
    delay(200);
    B = analogRead2() - 80;
    delay(200);
    Serial.print("Calibration: ");
    Serial.print(F);
    Serial.print(" | ");
    Serial.println(B);
  } else if ((F - readD1) > 200) {
    F = analogRead1() - 80;
    delay(200);
    B = analogRead2() - 80;
    delay(200);
    Serial.print("Calibration: ");
    Serial.print(F);
    Serial.print(" | ");
    Serial.println(B);
  }

  if (readD2 < B && readD1 < F) {
    delay(1000);
    lcd.clear();
    lcd.print("Please DO NOT block the corridor!!");
    delay(2000);
    lcd.clear();
    lcd.print(amount);
    delay(1000);
    return;
  } else {
    if (readD2 < B) { // A0 300 A1 100
      record("B");
    }
    if (readD1 < F) { // A0 100 A1 300
      record("F");
    }
  }
}

void getAndSendData() { // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Number\":";
  payload += amount;
  payload += "}";
  char attributes[1000];
  payload.toCharArray( attributes, 1000 );
  client.publish("v1/devices/me/telemetry", attributes);
  Serial.println(attributes);
}

void print_analog(String no) {
  Serial.print(no);
  Serial.print(". F: ");
  Serial.print(analogRead1());
  Serial.print(" | B: ");
  Serial.println(analogRead2());
}

void print_distance(String no, int val) {
  Serial.print("Distance");
  Serial.print(no);
  Serial.print(": ");
  Serial.print(val);
  Serial.println(" cm");
}

void check_distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
}

void record(String str) {
  record_d = record_d + str;
  Serial.println(record_d);
  if (record_d == "F") {
    check_distance();
    print_analog("1");
    distance2 = duration * 0.034 / 2;
    print_distance("2", distance2);
    delay(1000);
    return;
  } else if (record_d == "FB") {
    check_distance();
    distance3 = duration * 0.034 / 2;
    print_analog("2");
    print_distance("3", distance3);

    if (distance3 < distance2) {
      lcd.clear();
      amount += 1;
      lcd.print(amount);
      getAndSendData();
      Serial.println(amount);
      setItOnce = false;
    } else {
      Serial.println("invalid");
    }
    delay(1000);
    record_d = "";
    return;
  } else if (record_d == "B") {
    check_distance();
    print_analog("3");
    distance2 = duration * 0.034 / 2;
    print_distance("2", distance2);
    delay(1000);
    return;
  } else if (record_d == "BF") {
    check_distance();
    distance3 = duration * 0.034 / 2;
    print_analog("4");
    print_distance("3", distance3);

    if (distance2 < distance3) {
      lcd.clear();
      amount -= 1;
      lcd.print(amount);
      getAndSendData();
      Serial.println(amount);
      setItOnce = false;
    } else {
      Serial.println("invalid");
    }
    delay(1000);
    record_d = "";
    return;
  }  else if (record_d == "FF") {
    Serial.println("Cancel (Front and Front)");
    delay(1000);
    record_d = "";
  }  else if (record_d == "BB") {
    Serial.println("Cancel (Back and Back)");
    delay(1000);
    record_d = "";
  } else {
    Serial.println("invalid");
    delay(1000);
    record_d = "";
  }
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
