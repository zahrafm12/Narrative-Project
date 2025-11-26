#include <Servo.h>

// Servos
Servo linguiniServo;    // Scene 2: Linguini holds jar (Pin 10)
Servo remyArmServo;     // Scene 3: Remy pulls hat (Pin 6)
Servo pantryDoorServo;  // Scene 5: Pantry door opens (Pin 4)

// LEDs
const int scene2LedPin = 9;   // LED indicating Scene 2 is ready
const int hatLedPin = 13;     // Light in Scene 4 (Hat LED)

// Sensors (INPUT_PULLUP used, so LOW is triggered/pressed)
const int boatSensorPin = 12;   // Scene 1 trigger
const int jarSensorPin = 8;     // Scene 2 trigger
const int hatSensorPin = 7;     // Scene 3 trigger
const int switchSensorPin = 3;  // Scene 4 trigger
const int pantrySensorPin = 5;  // Scene 5 trigger

// Scene tracking
int currentScene = 1;         
int boatWasPressed = 0;     

// Linguini servo control (Scene 2)
bool linguiniTurned = false;        
bool jarSensorReady = false;        
int lastJarState = HIGH;            

// Hat sensor tracking (Scene 3)
bool hatWasPressed = false;         
bool scene3MovementComplete = false; // NEW FLAG: Prevents repeated movement and controls scene advance

void setup() {
  Serial.begin(9600);
  Serial.println("Ratatouille Scene Controller: Starting up...");

  // attach servos
  linguiniServo.attach(10);
  remyArmServo.attach(6);
  pantryDoorServo.attach(4);

  // Set standard servos to center position (90) or neutral/stop for continuous.
  // Assuming these are standard servos for position control.
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

  // ---------- Scene 1: Boat Action ----------
  if (currentScene == 1) {
    int boatState = digitalRead(boatSensorPin);
    
    // Detect press (LOW)
    if (boatState == LOW && boatWasPressed == 0) {
      Serial.println("Boat sensor touched… waiting for release.");
      boatWasPressed = 1;
    }
    
    // Detect release (HIGH) after press
    if (boatState == HIGH && boatWasPressed == 1) {
      Serial.println("SCENE 1 COMPLETE: Boat action finished.");
      digitalWrite(scene2LedPin, HIGH); // Signal ready for Scene 2
      currentScene = 2;
      boatWasPressed = 0;
      jarSensorReady = false; 
      Serial.println("Waiting for Scene 2 trigger (jar removed).");
    }
  }

  // ---------- Scene 2: Jar Removal / Linguini Action ----------
  else if (currentScene == 2) {
    int jarState = digitalRead(jarSensorPin);

    if (!jarSensorReady) {
        lastJarState = jarState;
        jarSensorReady = true;
    }

    // Trigger only on jar removal (LOW to HIGH transition)
    if (jarState == HIGH && lastJarState == LOW && !linguiniTurned) {
      
        Serial.println("SCENE 2 TRIGGERED: Jar removed, Linguini servo activated!");
        linguiniTurned = true;

        // Run Linguini's movement
        linguiniServo.write(0);
        delay(450);
        linguiniServo.write(90); // stop

        // Prepare for Scene 3
        remyArmServo.write(180); 
        currentScene = 3; 
        scene3MovementComplete = false; // Reset Scene 3 flag
        
        Serial.println("SCENE 2 COMPLETE: Moving to Scene 3.");
        Serial.println("Waiting for hat sensor trigger (Scene 3).");
    }

    lastJarState = jarState;
  }

  // ---------- Scene 3: Hat Wiggle / Remy Action ----------
  else if (currentScene == 3) {
    int hatState = digitalRead(hatSensorPin);

    // 1. Run the wiggle movement only once when the hat is pressed (LOW)
    if (hatState == LOW && !hatWasPressed && !scene3MovementComplete) {
        Serial.println("SCENE 3 TRIGGERED: Hat sensor activated, initiating wiggle!");
        
        hatWasPressed = true; // Mark that a press has been detected

        // The wiggle sequence
        for (int i = 0; i < 7; i++) {
          remyArmServo.write(140);
          delay(300);
          remyArmServo.write(220);
          delay(300);
        }

        remyArmServo.write(180);  // return to horizontal
        scene3MovementComplete = true; // Movement is now finished.

        Serial.println("SCENE 3 MOVEMENT COMPLETE. Waiting for hat release to advance.");
    }

    // 2. ADVANCE SCENE: Only advance to Scene 4 after the hat sensor is RELEASED (HIGH) 
    //    and the movement is complete. This fixes the concurrent activation bug.
    if (hatState == HIGH && scene3MovementComplete) {
      
      // Reset press tracking for future use (if scene 3 were revisited)
      hatWasPressed = false;
      
      currentScene = 4;
      Serial.println("SCENE 3 COMPLETE: Hat released. Moving to Scene 4.");
      Serial.println("Waiting for switch sensor trigger (Scene 4).");
    }
    
    // Safety: reset hatWasPressed if released prematurely
    if (hatState == HIGH && hatWasPressed) {
      hatWasPressed = false;
    }
  }

  // ---------- Scene 4: Light Switch Activation ----------
  else if (currentScene == 4) {
    int switchState = digitalRead(switchSensorPin);
    
    // Detect switch press (LOW)
    if (switchState == LOW) { 
      Serial.println("SCENE 4 TRIGGERED: Switch activated, turning on hat LED");
      digitalWrite(hatLedPin, HIGH); // turn on hat LED now
      currentScene = 5;
      Serial.println("SCENE 4 COMPLETE: Moving to Scene 5.");
    }
  }

  // ---------- Scene 5: Pantry Door Action ----------
  else if (currentScene == 5) {
    if (digitalRead(pantrySensorPin) == LOW) {
      Serial.println("SCENE 5 TRIGGERED: Pantry sensor activated.");
      pantryDoorServo.write(180); // Assuming 180 is open, 90 is closed. (Original had 90 here)
      currentScene = 6;
      Serial.println("STORY COMPLETE!");
    }
  }

  // ---------- Scene 6: End State ----------
  else if (currentScene == 6) {
    // do nothing, end of show
  }

}
