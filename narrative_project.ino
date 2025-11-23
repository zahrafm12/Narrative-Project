#include <Servo.h>

// Servos
Servo linguiniServo;   // Scene 2: Linguini holds jar
Servo remyArmServo;    // Scene 3: Remy pulls hair
Servo pantryDoorServo; // Scene 5: Pantry door opens

// LEDs
const int scene2LedPin = 9;  // Scene 1: LED that turns ON
const int hatLedPin = 13;    // Scene 4: LED for Remy shadow

// Sensors (copper tape buttons)
const int boatSensorPin = 12;    // Scene 1: Boat reaches destination
const int jarSensorPin = 8;      // Scene 2: Jar is pulled up / removed
const int hatSensorPin = 7;      // Scene 3: Hat is placed on Remy
const int switchSensorPin = 3;   // Scene 4: Chef touches light switch
const int pantrySensorPin = 5;   // Scene 5: Chef touches pantry door

// Scene tracking
int currentScene = 1;       
int boatWasPressed = 0;    

// Linguini servo control
bool linguiniTurned = false;      // Has the servo spun already?
bool jarSensorReady = false;      // Initialize sensor at scene start
int lastJarState = HIGH;          // Track previous jar sensor reading

void setup() {
  Serial.begin(9600);
  Serial.println("Ratatouille Scene Controller: Starting up...");

  // attach servos
  linguiniServo.attach(10);
  remyArmServo.attach(6);
  pantryDoorServo.attach(4);

  // STOP Linguini servo at start
  linguiniServo.write(90);

  // set LED pins
  pinMode(scene2LedPin, OUTPUT);
  pinMode(hatLedPin, OUTPUT);

  // set sensor pins
  pinMode(boatSensorPin, INPUT_PULLUP);
  pinMode(jarSensorPin, INPUT_PULLUP);
  pinMode(hatSensorPin, INPUT_PULLUP);
  pinMode(switchSensorPin, INPUT_PULLUP);
  pinMode(pantrySensorPin, INPUT_PULLUP);

  // initial positions
  digitalWrite(scene2LedPin, LOW);
  digitalWrite(hatLedPin, LOW);

  remyArmServo.write(0);      // arms down
  pantryDoorServo.write(90);  // door closed

  Serial.println("Setup complete. Waiting for Scene 1 trigger.");
  Serial.println("---------------------------------------");
}

void loop() {

  // ---------- Scene 1 ----------
  if (currentScene == 1) {
    int boatState = digitalRead(boatSensorPin);

    if (boatState == LOW && boatWasPressed == 0) {
      Serial.println("Boat sensor touched… waiting for release.");
      boatWasPressed = 1;
    }

    if (boatState == HIGH && boatWasPressed == 1) {
      Serial.println("SCENE 1 COMPLETE: Boat action finished.");
      digitalWrite(scene2LedPin, HIGH); // turn LED ON
      currentScene = 2;
      boatWasPressed = 0;
      Serial.println("Now waiting for Scene 2 trigger (jar removed).");
      Serial.println("---------------------------------------");
      
      // Scene 2 starts, initialize jar sensor
      jarSensorReady = false;  // ignore initial jar position
      delay(300);
    }
  }

  // ---------- Scene 2 ----------
  else if (currentScene == 2) {
    int jarState = digitalRead(jarSensorPin);

    // Initialize sensor reading at scene start
    if (!jarSensorReady) {
        lastJarState = jarState; // ignore the initial LOW state
        jarSensorReady = true;
    }

    // FALLING EDGE: LOW -> HIGH (jar removed)
    if (jarState == HIGH && lastJarState == LOW && !linguiniTurned) {
        Serial.println("SCENE 2 TRIGGERED: Jar removed, Linguini servo activated!");

        linguiniTurned = true; // mark as done

        // Spin servo once
        linguiniServo.write(0);   // full speed forward
        delay(900);               // adjust timing for ~360°
        linguiniServo.write(90);  // stop servo

        currentScene = 3;
        Serial.println("Linguini servo finished spinning. Move to Scene 3.");
        delay(500);
    }

    lastJarState = jarState; // save state for next loop
  }

  // ---------- Scene 3 ----------
  else if (currentScene == 3) {
    if (digitalRead(hatSensorPin) == LOW) {
      Serial.println("SCENE 3 TRIGGERED: Hat sensor activated.");
      remyArmServo.write(90); 
      currentScene = 4; 
      Serial.println("Now waiting for Scene 4 trigger (lightswitch).");
      Serial.println("---------------------------------------");
      delay(500);
    }
  }

  // ---------- Scene 4 ----------
  else if (currentScene == 4) {
    if (digitalRead(switchSensorPin) == LOW) {
      Serial.println("SCENE 4 TRIGGERED: Lightswitch sensor activated.");
      digitalWrite(hatLedPin, HIGH); 
      currentScene = 5; 
      Serial.println("Now waiting for Scene 5 trigger (pantry).");
      Serial.println("---------------------------------------");
      delay(500);
    }
  }

  // ---------- Scene 5 ----------
  else if (currentScene == 5) {
    if (digitalRead(pantrySensorPin) == LOW) {
      Serial.println("SCENE 5 TRIGGERED: Pantry sensor activated.");
      pantryDoorServo.write(90); // open door (adjust as needed)
      currentScene = 6; 
      Serial.println("---------------------------------------");
      Serial.println("STORY COMPLETE! All scenes finished.");
      Serial.println("---------------------------------------");
      delay(500);
    }
  }

  // ---------- Scene 6 ----------
  else if (currentScene == 6) {
    // do nothing, waiting for reset
  }

} // end loop



