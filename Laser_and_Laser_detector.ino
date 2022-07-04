#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define echoPin 2 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 3 //attach pin D3 Arduino to pin Trig of HC-SR04 
LiquidCrystal_I2C lcd(0x27, 20, 4);

long amount;
long F;
long B;
long duration;
int distance1;
int distance2;
int distance3;
bool setItOnce = false;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  amount = 0;
  pinMode(5, INPUT);
  pinMode(4, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.print("Counting...");
  F = analogRead(A0) - 50;
  B = analogRead(A1) - 50;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance1 = duration * 0.034 / 2 - 50;
  Serial.begin(9600);
  Serial.print(analogRead(A0));
  Serial.print(" | ");
  Serial.print(analogRead(A1));
  if (distance1 != 0) {
    Serial.print(" | Distance1: ");
    Serial.print(distance1);
    Serial.println(" cm");
  }
}

void loop() {
  if (analogRead(A1) < B && analogRead(A0) < F) {
    delay(500);
    lcd.clear();
    lcd.print("Please DO NOT block the corridor!!");
    delay(2000);
    lcd.clear();
    lcd.print(amount);
    delay(1000);
    return;
  } else {
    if (analogRead(A1) < B) { // A0 300 A1 100
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
      Serial.print("1. ");
      Serial.print(analogRead(A0));
      Serial.print(" | ");
      Serial.println(analogRead(A1));
      Serial.print("Distance2: ");
      Serial.print(distance2);
      Serial.println(" cm");
      delay(300);
      if (analogRead(A0) < F) { // A0 100
        distance3 = duration * 0.034 / 2;
        Serial.print("2. ");
        Serial.print(analogRead(A0));
        Serial.print(" | ");
        Serial.println(analogRead(A1));
        Serial.print("Distance3: ");
        Serial.print(distance3);
        Serial.println(" cm");
        if (distance3 < distance2) {
          lcd.clear();
          amount += 1;
          lcd.print(amount);
          Serial.println(amount);
          delay(1000);
          setItOnce = false;
          return;
        }
      }
    }
    if (analogRead(A0) < F) { // A0 100 A1 300
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
      Serial.print("3. ");
      Serial.print(analogRead(A0));
      Serial.print(" | ");
      Serial.println(analogRead(A1));
      Serial.print("Distance2: ");
      Serial.print(distance2);
      Serial.println(" cm");
      delay(300);
      if (analogRead(A1) < B) { // A1 100
        distance3 = duration * 0.034 / 2;
        Serial.print("4. ");
        Serial.print(analogRead(A0));
        Serial.print(" | ");
        Serial.println(analogRead(A1));
        Serial.print("Distance3: ");
        Serial.print(distance3);
        Serial.println(" cm");
        if (distance2 < distance3) {
          lcd.clear();
          amount -= 1;
          lcd.print(amount);
          Serial.println(amount);
          delay(1000);
          setItOnce = false;
          return;
        }
      }
    }
  }
}
