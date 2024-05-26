#include <TimerOne.h>
#define trigger A3 // trigger pin
#define echo A2 // echo pin
#define EN_A 10 // Enable pin for first motor
#define IN1 9 // control pin for first motor
#define IN2 8 // control pin for first motor
#define IN3 7 // control pin for second motor
#define IN4 6 // control pin for second motor
#define EN_B 5 // Enable pin for second motor
#define LED 3 // led pin
int distance; // distance in cm
volatile int fadeValue = 0; // PWM value for LED brightness
volatile int fadeAmount = 5; // Amount to change the brightness each time
volatile bool isFading = false; // Flag to indicate if fading is active

long Ultrasonic_read() {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  long time = pulseIn (echo, HIGH);
  return time / 29 / 2 ; // transform to cm
}

void turnLeft(){ //turnLeft
  digitalWrite(IN1, HIGH); // Left Motor backward Pin 
  digitalWrite(IN2, LOW); // Left Motor forward Pin 
  digitalWrite(IN3, HIGH); // Right Motor forward Pin 
  digitalWrite(IN4, LOW); // Right Motor backward Pin 
}

void turnRight(){ //turnRight
  digitalWrite(IN1, LOW); // Left Motor backward Pin 
  digitalWrite(IN2, HIGH); // Left Motor forward Pin 
  digitalWrite(IN3, LOW); // Right Motor forward Pin 
  digitalWrite(IN4, HIGH); // Right Motor backward Pin 
}

void backward(){  // backward
  digitalWrite(IN1, LOW); // Left Motor backward Pin 
  digitalWrite(IN2, HIGH); // Left Motor forward Pin 
  digitalWrite(IN3, HIGH); // Right Motor forward Pin 
  digitalWrite(IN4, LOW); // Right Motor backward Pin 
}

void forward(){ // forward
  digitalWrite(IN1, HIGH); // Left Motor backward Pin 
  digitalWrite(IN2, LOW); // Left Motor forward Pin 
  digitalWrite(IN3, LOW); // Right Motor forward Pin 
  digitalWrite(IN4, HIGH); // Right Motor backward Pin 
}

void stop(){ //stop
  digitalWrite(IN1, LOW); // Left Motor backward Pin 
  digitalWrite(IN2, LOW); // Left Motor forward Pin 
  digitalWrite(IN3, LOW); // Right Motor forward Pin 
  digitalWrite(IN4, LOW); // Right Motor backward Pin 
}

int checkRightDirection() {
  turnRight();
  manevreLed();
  stop();
  delay(100);
  return Ultrasonic_read();
}

int checkLeftDirection() {
  turnLeft();
  manevreLed();
  manevreLed();
  stop();
  return Ultrasonic_read();
}

void returnToInitialPosition() {
  turnRight();
  manevreLed();
  stop();
  delay(200);
}

void obstacleLed() {
  analogWrite(LED, 70);
  delay(1000);
}

void manevreLed() {
  for(int i = 2; i >= 0; i--) {
    analogWrite(LED, 70);
    delay(50);
    analogWrite(LED, 0);
    delay(50);
  }
}

void fadeLED() {
  fadeValue += fadeAmount;
  if (fadeValue <= 0 || fadeValue >= 255) {
    fadeAmount = -fadeAmount; // Reverse direction of fading
  }
  analogWrite(LED, fadeValue); // Set the PWM value
}

void startFade() {
  if (!isFading) {
    Timer1.attachInterrupt(fadeLED); // Attach the service routine here
    isFading = true;
  }
}

void stopFade() {
  if (isFading) {
    Timer1.detachInterrupt(); // Detach the interrupt to stop fading
    isFading = false;
    analogWrite(LED, 0); // Optionally turn off the LED
  }
}

void setup() {
  pinMode(trigger, OUTPUT); // declare ultrasonic sensor Trigger pin as Output  
	pinMode(echo, INPUT );// declare ultrasonic sensor Echo pin as input
  Serial.begin (9600); //Starting the serial communication at 9600 baud rate
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(LED, OUTPUT);
  Timer1.initialize(10000);  // Set a timer of 10000 microseconds (0.01 seconds)
  distance = Ultrasonic_read();
  delay(500);
}


void loop() {
  distance = Ultrasonic_read();
  Serial.print("Distance: ");  
	Serial.println(distance);
 
  if (distance > 25) {
    forward();
    startFade();
  } else {
    distance = Ultrasonic_read();
    stop();
	  Serial.println("stop");
    stopFade();
    obstacleLed();
    backward();
    Serial.println("backward");
    manevreLed();
    distance = Ultrasonic_read();
    stop();
    delay(200);
    int right = checkRightDirection();
    int left = checkLeftDirection();
    returnToInitialPosition();
  
    if (right >= left) {
	    Serial.println("right");
      turnRight();
      manevreLed();
    } else if (left > right) {
      Serial.println("left");
      turnRight();
      manevreLed();
    }
  }
  delay(10);
}

