// ---------------------------------------------------------------- //
// Arduino Ultrasoninc Sensor HC-SR04
// Re-writed by Arbi Abdul Jabbaar
// Using Arduino IDE 1.8.7
// Using HC-SR04 Module
// Tested on 17 September 2019
// ---------------------------------------------------------------- //

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


void loop() {
s1 = digitalRead(state1); // TODO: connect s1 and s0 to the output of the fob encoder
s0 = digitalRead(state0);
//Serial.print("s1: ");
//Serial.println(s1);
//Serial.print("s0: ");
//Serial.println(s0);
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