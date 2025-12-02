#include <Servo.h>

// Servos
Servo linguiniServo;    
Servo remyArmServo;     
Servo pantryDoorServo;  

// LEDs
const int scene2LedPin = 9;
const int hatLedPin = 2;

// Sensors (LOW = pressed)
const int boatSensorPin = 12;
const int jarSensorPin = 8;
const int hatSensorPin = 7;
const int switchSensorPin = 3;
const int pantrySensorPin = 5;

// Scene tracking
int currentScene = 1;
bool boatWasPressed = false;

// Scene 2 tracking
bool linguiniTurned = false;
bool jarSensorReady = false;
int lastJarState = HIGH;

// Scene 3 tracking
bool hatWasPressed = false;
bool scene3MovementComplete = false;

// Scene 5 tracking
bool pantryOpened = false;

void setup() {

  Serial.begin(9600);
  Serial.println("Ratatouille Scene Controller: Starting up...");

  linguiniServo.attach(11);
  remyArmServo.attach(6);
  pantryDoorServo.attach(4);

  // Starting positions
  linguiniServo.write(90);
  remyArmServo.write(180);
  pantryDoorServo.write(0);

  // Detach pantry servo so it cannot move early
  pantryDoorServo.detach();

  pinMode(scene2LedPin, OUTPUT);
  pinMode(hatLedPin, OUTPUT);

  pinMode(boatSensorPin, INPUT_PULLUP);
  pinMode(jarSensorPin, INPUT_PULLUP);
  pinMode(hatSensorPin, INPUT_PULLUP);
  pinMode(switchSensorPin, INPUT_PULLUP);
  pinMode(pantrySensorPin, INPUT_PULLUP);

  // LEDs start OFF
  digitalWrite(scene2LedPin, LOW);
  digitalWrite(hatLedPin, LOW);

  Serial.println("Setup complete.");
}

void loop() {

  // ---------- Scene 1 ----------
  if (currentScene == 1) {

    int boatState = digitalRead(boatSensorPin);

    if (boatState == LOW && !boatWasPressed) {
      boatWasPressed = true;
      Serial.println("Scene 1: Boat pressed.");
    }

    if (boatState == HIGH && boatWasPressed) {
      digitalWrite(scene2LedPin, HIGH);
      currentScene = 2;
      boatWasPressed = false;
      jarSensorReady = false;
      Serial.println("Scene 1 COMPLETE → Scene 2");
    }
  }

  // ---------- Scene 2 ----------
  else if (currentScene == 2) {

    int jarState = digitalRead(jarSensorPin);

    if (!jarSensorReady) {
      lastJarState = jarState;
      jarSensorReady = true;
    }

    if (jarState == HIGH && lastJarState == LOW && !linguiniTurned) {

      Serial.println("Scene 2: Jar removed — moving Linguini");

      linguiniTurned = true;
      linguiniServo.write(0);
      delay(450);
      linguiniServo.write(90);

      remyArmServo.write(180);

      currentScene = 3;
      scene3MovementComplete = false;

      Serial.println("Scene 2 COMPLETE → Scene 3");
    }

    lastJarState = jarState;
  }

  // ---------- Scene 3 ----------
  else if (currentScene == 3) {

    digitalWrite(hatLedPin, LOW);

    if (pantryDoorServo.attached()) {
      pantryDoorServo.detach();
    }

    int hatState = digitalRead(hatSensorPin);

    if (hatState == LOW && !hatWasPressed && !scene3MovementComplete) {
      Serial.println("Scene 3: Hat pressed — wiggle.");

      hatWasPressed = true;

      for (int i = 0; i < 7; i++) {
        remyArmServo.write(140);
        delay(300);
        remyArmServo.write(220);
        delay(300);
      }

      remyArmServo.write(180);
      scene3MovementComplete = true;
    }

    if (scene3MovementComplete && hatState == HIGH) {
      hatWasPressed = false;
      currentScene = 4;
      Serial.println("Scene 3 COMPLETE → Scene 4");
    }
  }

  // ----------------------------------------------------
  // ---------- NEW SCENE 4 (Stable, No Flicker) --------
  // ----------------------------------------------------
  else if (currentScene == 4) {

    digitalWrite(hatLedPin, LOW);   // stays OFF unless switch pressed

    static bool switchLatched = false;

    int switchState = digitalRead(switchSensorPin);

    // Detect press only once
    if (switchState == LOW && !switchLatched) {

      delay(50); // debounce
      if (digitalRead(switchSensorPin) == LOW) {

        switchLatched = true;
        digitalWrite(hatLedPin, HIGH);

        Serial.println("SCENE 4: Switch pressed → LED ON");
        Serial.println("Scene 4 COMPLETE → Scene 5");

        currentScene = 5;
      }
    }
  }

  // ----------------------------------------------------
  // ---------- NEW SCENE 5 (Move Pantry 180 Once) ------
  // ----------------------------------------------------
  else if (currentScene == 5 && !pantryOpened) {

    int pantryState = digitalRead(pantrySensorPin);

    if (pantryState == LOW) {

      delay(50); // debounce
      if (digitalRead(pantrySensorPin) == LOW) {

        Serial.println("SCENE 5: Pantry triggered!");

        pantryDoorServo.attach(4);
        pantryDoorServo.write(180);     // full open
        delay(700);

        pantryDoorServo.detach();
        pantryOpened = true;

        Serial.println("Scene 5 COMPLETE → Scene 6");
        currentScene = 6;
      }
    }
  }

  // ---------- Scene 6 ----------
  else if (currentScene == 6) {
    // End state — nothing moves
  }
}
