// www.edisonsciencecorner.blogspot.com
// https://www.youtube.com/channel/UCESZHuYTzx0Eyd3yJ8u60Fg

#include <ESP8266WiFi.h>
char ssid[] = "edison science corner"; // ssid
char pass[] = "eeeeeeee"; // password
int readD1;
int readD2;
int Pin_D1 = 4;
int Pin_D2 = 5;
 
void setup() {
    Serial.begin(9600);
    pinMode(Pin_D1, OUTPUT);
    pinMode(Pin_D2, OUTPUT);
  
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
    readD1 = analogRead1(); // Read Analog value of first sensor
    delay(200);
    readD2 = analogRead2(); // Read Analog value of second sensor
    delay(200);
    Serial.print("sensor 1 = ");
    Serial.print(readD1);
    Serial.print(" / sensor 2 = ");
    Serial.println(readD2);
 
}