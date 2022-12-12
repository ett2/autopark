/*
--------------------------------------------------------------------------
Object Detection Module
--------------------------------------------------------------------------
License:   
Copyright 2022 Eunice Tan
Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this 
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors 
may be used to endorse or promote products derived from this software without 
specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
--------------------------------------------------------------------------
This does a few things:
- Checks Distance from multiple HCSR04 sensors
- If an object is detected under a certain threshold, pulls up a pin and lights up an LED
--------------------------------------------------------------------------
Adapted from:
https://create.arduino.cc/projecthub/abdularbi17/ultrasonic-sensor-hc-sr04-with-arduino-tutorial-327ff6

*/


// ------------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------------

// Pin setup
#define trig0 5
#define echo0 6 
#define trig1 7
#define echo1 8
#define trig2 9
#define echo2 10
#define flag  4
#define state1 A5
#define state0 A4

// Variables
long duration; // duration of sound wave travel
int distance; // distance measurement
int flagstate; // emergency brake flag
int s1;
int s0;

// ------------------------------------------------------------------------
// Setup
// ------------------------------------------------------------------------
void setup() {
  pinMode(trig0, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echo0, INPUT); // Sets the echoPin as an INPUT
  pinMode(trig1, OUTPUT); 
  pinMode(echo1, INPUT); 
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT); 
  pinMode(flag, OUTPUT);
  pinMode(state1, INPUT); 
  pinMode(state0, INPUT);

  Serial.begin(9600); 
  Serial.println("Ultrasonic Sensor Emergency Braking State System"); 
  Serial.println("Setup complete :)");
}

// ------------------------------------------------------------------------
// Main Loop
// ------------------------------------------------------------------------
void loop() {
s1 = digitalRead(state1); 
s0 = digitalRead(state0);
//Serial.print("s1: ");
//Serial.println(s1);
//Serial.print("s0: ");
//Serial.println(s0);

// only detects objects in a certain state
if (s1 == 1 && s0 == 0) {
  ultrasonic(trig0, echo0);
  delay(500);
  distancecheck(distance);
  delay(500);
  ultrasonic(trig1, echo1);
  delay(500);
  distancecheck(distance);
  delay(500);
  //ultrasonic(trig2, echo2);
  //delay(1000);
  //distancecheck(distance);
  //delay(1000);
} else {
  digitalWrite(flag, LOW);
  delay(500);
}
}

// ------------------------------------------------------------------------
// Ultrasonic Sensor Distance Measurement
// ------------------------------------------------------------------------

void ultrasonic(int trig, int echo) {
  // Clears the trigPin condition
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

// ------------------------------------------------------------------------
// Output driver
// ------------------------------------------------------------------------
void distancecheck(int distance) {
  if (distance < 30) { // change this value to make the braking more sensitive
    digitalWrite(flag, HIGH);
    Serial.println("Emergency Brake!");
    delay(10000); // gives time for obstacle to get out of the way
  }
  else {
    digitalWrite(flag, LOW);
  }
}