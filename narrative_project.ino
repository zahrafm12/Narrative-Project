#include <Servo.h>

// Servos
Servo linguiniServo;    // Scene 2: Linguini holds jar (Pin 10)
Servo remyArmServo;     // Scene 3: Remy pulls hat (Pin 6)
Servo pantryDoorServo;  // Scene 5: Pantry door opens (Pin 4)

// LEDs
const int scene2LedPin = 9;
const int hatLedPin = 2; // Hat LED (Pin 2)

// Sensors (LOW = pressed)
const int boatSensorPin = 12;
const int jarSensorPin = 8;
const int hatSensorPin = 7;
const int switchSensorPin = 3;  // Scene 4 trigger
const int pantrySensorPin = 5;  // Scene 5 trigger

// Scene tracking
int currentScene = 1;
bool boatWasPressed = false;  // track Scene 1 press

// Scene 2 tracking
bool linguiniTurned = false;
bool jarSensorReady = false;
int lastJarState = HIGH;

// Scene 3 tracking
bool hatWasPressed = false;
bool scene3MovementComplete = false;

// Scene 5 tracking (NEW FLAG to manage single-movement and prevent spinning)
bool pantryOpened = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Ratatouille Scene Controller: Starting up...");

  linguiniServo.attach(10);
  remyArmServo.attach(6);
  pantryDoorServo.attach(4);

  // Starting positions
  linguiniServo.write(90);    // Neutral position
  remyArmServo.write(180);    // Horizontal (closed) position
  pantryDoorServo.write(0);   // Door closed (0 degrees is the assumed closed position)
  
  // Detach servos that are not needed immediately to prevent spinning/jitter
  pantryDoorServo.detach();
  
  Serial.println("Initial servos set and detached. Waiting for Scene 1 trigger.");


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
}

void loop() {

  // ---------- Scene 1: Boat ----------
  if (currentScene == 1) {
    int boatState = digitalRead(boatSensorPin);

    // Detect press
    if (boatState == LOW && !boatWasPressed) {
      boatWasPressed = true;
      Serial.println("Scene 1: Boat press detected.");
    }

    // Detect release after press
    if (boatState == HIGH && boatWasPressed) {
      digitalWrite(scene2LedPin, HIGH); // Signal ready for Scene 2
      currentScene = 2;
      boatWasPressed = false;
      jarSensorReady = false;
      Serial.println("Scene 1 COMPLETE. Moving to Scene 2.");
    }
  }

  // ---------- Scene 2: Jar/Linguini ----------
  else if (currentScene == 2) {
    int jarState = digitalRead(jarSensorPin);

    if (!jarSensorReady) {
      lastJarState = jarState;
      jarSensorReady = true;
    }

    // Trigger on jar removal (LOW to HIGH transition)
    if (jarState == HIGH && lastJarState == LOW && !linguiniTurned) {
      Serial.println("Scene 2: Jar removed, Linguini moving.");
      linguiniTurned = true;

      linguiniServo.write(0);
      delay(450);
      linguiniServo.write(90); // Stop movement

      // Ensure Remy arm is in start position
      remyArmServo.write(180);

      currentScene = 3;
      scene3MovementComplete = false;
      Serial.println("Scene 2 COMPLETE. Moving to Scene 3.");
    }

    lastJarState = jarState;
  }

  // ---------- Scene 3: Hat Wiggle ----------
else if (currentScene == 3) {
    // Ensure hat LED is OFF during Scene 3
    digitalWrite(hatLedPin, LOW);

    // Ensure pantry servo is not attached
    if (pantryDoorServo.attached()) {
        pantryDoorServo.detach(); 
    }

    int hatState = digitalRead(hatSensorPin);

    // Trigger wiggle once
    if (hatState == LOW && !hatWasPressed && !scene3MovementComplete) {
        Serial.println("Scene 3: Hat sensor activated, wiggling.");
        hatWasPressed = true;

        for (int i = 0; i < 7; i++) {
            remyArmServo.write(140);
            delay(300);
            remyArmServo.write(220);
            delay(300);
        }

        remyArmServo.write(180); // Return to neutral
        scene3MovementComplete = true;
    }

    // Advance scene only when hat released after wiggle
    if (scene3MovementComplete && hatState == HIGH) {
        hatWasPressed = false;
        currentScene = 4;
        Serial.println("Scene 3 COMPLETE. Moving to Scene 4.");
    }
}

// ---------- Scene 4: Switch LED (Fixed: LED stays ON) ----------
else if (currentScene == 4) {
    // NOTE: The LED is kept OFF by default until the switch is pressed.
    
    int switchState = digitalRead(switchSensorPin);
    
    // Check if the switch is pressed (LOW)
    if (switchState == LOW) {
        delay(50); // debounce
        if (digitalRead(switchSensorPin) == LOW) {
            
            // ACTION: Turn the LED ON permanently
            digitalWrite(hatLedPin, HIGH); 
            Serial.println("SCENE 4: Hat LED activated. Moving to Scene 5.");
            
            currentScene = 5;
        }
    }
}


  // ---------- Scene 5: Pantry Door (Fixed: Single movement and no spinning) ----------
  else if (currentScene == 5 && !pantryOpened) {
    int pantryState = digitalRead(pantrySensorPin);
    
    // Detect sensor trigger (LOW)
    if (pantryState == LOW) {
      delay(50); // Debounce delay
      if (digitalRead(pantrySensorPin) == LOW) {
        
        Serial.println("SCENE 5 TRIGGERED: Pantry sensor activated.");
        
        // CRITICAL: Re-attach the servo only when movement is needed.
        pantryDoorServo.attach(4);
        
        // ACTION: Move servo 90 degrees (open) exactly once.
        pantryDoorServo.write(90); 
        pantryOpened = true;
        Serial.println("Scene 5: pantry door OPEN (90 degrees)");

        // Wait briefly for the servo to complete the physical movement.
        delay(500); 

        // CRITICAL: Detach the servo to stop the PWM signal immediately and prevent spinning.
        pantryDoorServo.detach(); 
        Serial.println("Servo detached. Movement complete.");

        currentScene = 6; // Advance to the end scene
      }
    }
  }

  // ---------- Scene 6: End State ----------
  else if (currentScene == 6) {
    // Only print once upon entering the end state
    if (pantryOpened) {
      Serial.println("STORY COMPLETE!");
      pantryOpened = false; 
    }
    // All action stops here.
  }
}
