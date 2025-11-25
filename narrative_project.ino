#include <Servo.h>

// Servos
Servo linguiniServo;   // Scene 2: Linguini holds jar
Servo remyArmServo;    // Scene 3: Remy pulls hat
Servo pantryDoorServo; // Scene 5: Pantry door opens

// LEDs
const int scene2LedPin = 9;  
const int hatLedPin = 13;    

// Sensors
const int boatSensorPin = 12;    
const int jarSensorPin = 8;      
const int hatSensorPin = 7;      
const int switchSensorPin = 3;   
const int pantrySensorPin = 5;   

// Scene tracking
int currentScene = 1;       
int boatWasPressed = 0;    

// Linguini servo control
bool linguiniTurned = false;      
bool jarSensorReady = false;      
int lastJarState = HIGH;          

// Hat sensor tracking
bool hatWasPressed = false;  // ensures wiggle only triggers on new touch

void setup() {
  Serial.begin(9600);
  Serial.println("Ratatouille Scene Controller: Starting up...");

  // attach servos
  linguiniServo.attach(10);
  remyArmServo.attach(6);
  pantryDoorServo.attach(4);

  // Stop continuous servo at start
  linguiniServo.write(90);

  // Remy/hat servo starts at horizontal 180°
  remyArmServo.write(180);

  // Pantry door closed
  pantryDoorServo.write(90); 

  // LED setup
  pinMode(scene2LedPin, OUTPUT);
  pinMode(hatLedPin, OUTPUT);

  // Sensor setup
  pinMode(boatSensorPin, INPUT_PULLUP);
  pinMode(jarSensorPin, INPUT_PULLUP);
  pinMode(hatSensorPin, INPUT_PULLUP);
  pinMode(switchSensorPin, INPUT_PULLUP);
  pinMode(pantrySensorPin, INPUT_PULLUP);

  // initial LED states
  digitalWrite(scene2LedPin, LOW);
  digitalWrite(hatLedPin, LOW);

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
      digitalWrite(scene2LedPin, HIGH);
      currentScene = 2;
      boatWasPressed = 0;
      jarSensorReady = false; // prepare Scene 2
      Serial.println("Waiting for Scene 2 trigger (jar removed).");
    }
  }

  // ---------- Scene 2 ----------
  else if (currentScene == 2) {
    int jarState = digitalRead(jarSensorPin);

    if (!jarSensorReady) {
        lastJarState = jarState;  // initialize sensor state
        jarSensorReady = true;
    }

    // Trigger only on jar removal
    if (jarState == HIGH && lastJarState == LOW && !linguiniTurned) {
        Serial.println("SCENE 2 TRIGGERED: Jar removed, Linguini servo activated!");
        linguiniTurned = true;

        linguiniServo.write(0);
        delay(450);
        linguiniServo.write(90); // stop

        // Remy servo remains stopped
        remyArmServo.write(180); 
        // Hat LED remains off

        currentScene = 3; // move to Scene 3
        Serial.println("Waiting for hat sensor trigger (Scene 3).");
    }

    lastJarState = jarState;
  }

  // ---------- Scene 3 ----------
  else if (currentScene == 3) {
    int hatState = digitalRead(hatSensorPin);

    if (hatState == LOW && !hatWasPressed) { // only wiggle on new press
        Serial.println("SCENE 3 TRIGGERED: Hat sensor activated!");
        hatWasPressed = true;

        for (int i = 0; i < 7; i++) {
            remyArmServo.write(140);
            delay(300);
            remyArmServo.write(220);
            delay(300);
        }

        remyArmServo.write(180);  // return to horizontal

        // Now that hat has been triggered, advance scene
        currentScene = 4;
        Serial.println("Waiting for switch sensor trigger (Scene 4).");
    }

    if (hatState == HIGH && hatWasPressed) {
        hatWasPressed = false; // reset press state
    }
  }

  // ---------- Scene 4 ----------
  else if (currentScene == 4) {
    int switchState = digitalRead(switchSensorPin);
    if (switchState == LOW) { 
        Serial.println("SCENE 4 TRIGGERED: Switch activated, turning on hat LED");
        digitalWrite(hatLedPin, HIGH); // turn on hat LED now
        currentScene = 5;
    }
  }

  // ---------- Scene 5 ----------
  else if (currentScene == 5) {
    if (digitalRead(pantrySensorPin) == LOW) {
        Serial.println("SCENE 5 TRIGGERED: Pantry sensor activated.");
        pantryDoorServo.write(90);
        currentScene = 6;
        Serial.println("STORY COMPLETE!");
    }
  }

  // ---------- Scene 6 ----------
  else if (currentScene == 6) {
    // do nothing, end of show
  }

}


