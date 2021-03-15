// ---------------------------------------------------------------- //
// Arduino Ultrasoninc Sensor HC-SR04 with LCD 16x2
// Re-writed by Arbi Abdul Jabbaar
// Using Arduino IDE 1.8.7
// Using HC-SR04 Module
// Tested on 17 September 2019
// ---------------------------------------------------------------- //
#include <LiquidCrystal.h>
LiquidCrystal lcd(6, 7, 8, 9, 10, 11); // RS, EN, D4, D5, D6, D7

#define echoPin 12 // attach pin D12 Arduino to pin Echo of HC-SR04
#define trigPin 13 //attach pin D13 Arduino to pin Trig of HC-SR04

// defines variables
long duration; // variable for the duration of sound wave travel
int distance_cm; // variable for centimeters measurement
int distance_inch; // variable for inches measurement

void setup() {
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
  lcd.begin(16, 2); // lcd starts with resolution 16x2
}
void loop() {
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
  distance_cm = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  distance_inch = duration * 0.0133 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(distance_cm);
  lcd.println(" cm");
  lcd.setCursor(0, 1);
  lcd.print("Distance: ");
  lcd.print(distance_inch);
  lcd.println(" inch");
}
