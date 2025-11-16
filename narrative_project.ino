#include <Servo.h>

// Servos
Servo linguiniServo;  // Scene 2: Linguini holds jar
Servo remyArmServo;    // Scene 3: Remy pulls hair
Servo pantryDoorServo; // Scene 5: Pantry door opens

// LEDs
const int scene2LedPin = 2;  // Scene 1: LED that turns ON
const int hatLedPin = 3;     // Scene 4: LED for Remy shadow

// SENSORS (coppor tape buttons)
const int boatSensorPin = 4;    // Scene 1: Boat reaches destination
const int jarSensorPin = 5;     // Scene 2: Jar is pulled up
const int hatSensorPin = 6;     // Scene 3: Hat is placed on Remy
const int switchSensorPin = 7;  // Scene 4: Chef guy touches lightswitch
const int pantrySensorPin = 8;  // Scene 5: Chef guy touches pantry door

int currentScene = 1; // Start at Scene 1

//pins setup
void setup() {
  Serial.begin(9600); //open serial monitor for debugging
  Serial.println("Ratatouille Scene Controller: Starting up...");

  //attach servos to their pins
  linguiniServo.attach(9);
  remyArmServo.attach(10);
  pantryDoorServo.attach(11);

  //set LED pins as OUTPUTS
  pinMode(scene2LedPin, OUTPUT);
  pinMode(hatLedPin, OUTPUT);

  //set all sensor pins as INPUT_PULLUP
  //pin HIGH by default
  //when copper tapes touch, the pin goes LOW.
  pinMode(boatSensorPin, INPUT_PULLUP);
  pinMode(jarSensorPin, INPUT_PULLUP);
  pinMode(hatSensorPin, INPUT_PULLUP);
  pinMode(switchSensorPin, INPUT_PULLUP);
  pinMode(pantrySensorPin, INPUT_PULLUP);

  //set initial positions for servos & LEDs 
  Serial.println("Setting initial scene state.");
  digitalWrite(scene2LedPin, LOW); // Turn LED off
  digitalWrite(hatLedPin, LOW);    // Turn hat LED off

//will calibrate
  linguiniServo.write(180); // Example: 180 = "arm down"
  remyArmServo.write(0);    // Example: 0 = "arms down"
  pantryDoorServo.write(90);  // Example: 90 = "door closed"

  Serial.println("Setup complete. Waiting for Scene 1 trigger.");
  Serial.println("---------------------------------------");
}

//main look checks current scene
void loop() {

  /* the loop() function runs over and over, but the
  'if' statements make sure it only pays attention
  to the sensor for the current scene. */

  if (currentScene == 1) {
    //code for scene 1
    //waiting for person to pull boat (boatSensorPin)
    
    if (digitalRead(boatSensorPin) == LOW) { //check if tapes are touching
      Serial.println("SCENE 1 TRIGGERED: Boat sensor activated.");
      
      //turn on LED for next scene
      digitalWrite(scene2LedPin, HIGH); 
      
      currentScene = 2; //move to the next scene
      Serial.println("Now waiting for Scene 2 trigger (jar).");
      Serial.println("---------------------------------------");
      delay(500); //wait half a second to prevent double-triggers
    }
  } 

  else if (currentScene == 2) {
    //code for scene 2
    //waiting for person to pull jar up (jarSensorPin)
    
    if (digitalRead(jarSensorPin) == LOW) { 
      Serial.println("SCENE 2 TRIGGERED: Jar sensor activated.");
      
      //move servo to show Linguini holding Remy
      //WILL CALIBRATE
      linguiniServo.write(90);
      
      currentScene = 3; //move to the next scene
      Serial.println("Now waiting for Scene 3 trigger (hat).");
      Serial.println("---------------------------------------");
      delay(500); 
    }
  } 

  else if (currentScene == 3) {
    //code for scene 3
    //waiting for person to put mini hat on Remy (hatSensorPin)
    
    if (digitalRead(hatSensorPin) == LOW) { 
      Serial.println("SCENE 3 TRIGGERED: Hat sensor activated.");

      // trigger Remy's arms to pull hair
      //WILL MAKE IT SO IT GOES UP AND DOWN FOR 6 SECONDS
      remyArmServo.write(90); 
      
      currentScene = 4; 
      Serial.println("Now waiting for Scene 4 trigger (lightswitch).");
      Serial.println("---------------------------------------");
      delay(500);
    }
  } 

  else if (currentScene == 4) {
    //code for scene 4
    //waiting for person to move little chef to lightswitch (switchSensorPin)
    
    if (digitalRead(switchSensorPin) == LOW) { 
      Serial.println("SCENE 4 TRIGGERED: Lightswitch sensor activated.");
      
      //turn on LED behind hat for shadow
      digitalWrite(hatLedPin, HIGH); 
      
      currentScene = 5; // Move to the next scene
      Serial.println("Now waiting for Scene 5 trigger (pantry).");
      Serial.println("---------------------------------------");
      delay(500); 
    }
  } 

  else if (currentScene == 5) {
    //code for scene 5
    //waiting for person to move chef to pantry door (pantrySensorPin)
    
    if (digitalRead(pantrySensorPin) == LOW) { 
      Serial.println("SCENE 5 TRIGGERED: Pantry sensor activated.");
      
      //open pantry door
      //WILL CALIBRATE
      pantryDoorServo.write(90);
      
      currentScene = 6; //move to end state
      Serial.println("---------------------------------------");
      Serial.println("STORY COMPLETE! All scenes finished.");
      Serial.println("---------------------------------------");
      delay(500); 
    }
  }

  else if (currentScene == 6) {
    //will do nothing until reset
  }
  
} //the end :)


//insert code for scene 1: person pulls boat across water, interaction turns LED in the next scene ON 
//insert code for scene 2: person pulls the jar up (with remy in it) to trigger servo moves to show linguini holding remy outside the jar.
//inset code for scene 3: person put a mini chef hat on remy, triggers remy's arms to pull linguini's hair
//insert code for scene 4: person moves small chef guy to a lightswitch on the wall, triggers the LED to turn on behind linguini's hat, shows Remy shadow. 
//insert code for scene 5: person moves littel chef to pantry door, physical door opens and reveals 3D pantry. 
