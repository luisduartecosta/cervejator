#include <AccelStepper.h>

#define ENABLE_1_GARRAFA 10
#define ENABLE_2_GARRAFA 11
#define LED_STATUS       53
#define BUTTON           52

AccelStepper motorCopo(4, 2, 3, 4, 5);   
AccelStepper motorGarrafa(4, 6, 7, 8, 9); 

int stepCount = 0;
bool serving = false;

float walk = -2000.0;

void setup() {
  pinMode(ENABLE_1_GARRAFA,OUTPUT);
  pinMode(ENABLE_2_GARRAFA,OUTPUT);
  pinMode(LED_STATUS,OUTPUT);
  pinMode(BUTTON, INPUT);
  
  motorCopo.setSpeed(200);
  motorGarrafa.setSpeed(200);
  
  motorCopo.setMaxSpeed(200.0);
  motorCopo.setAcceleration(30.0);
  motorCopo.moveTo(walk);
  
  motorGarrafa.setMaxSpeed(200.0);
  motorGarrafa.setAcceleration(30.0);
  motorGarrafa.moveTo(walk);
  
  serving = true;

  delay(1000);
  
  digitalWrite(LED_STATUS, LOW);
}

void loop() {
  digitalWrite(ENABLE_1_GARRAFA, HIGH);  // Enable 1 do driver
  digitalWrite(ENABLE_2_GARRAFA, HIGH);  // Enable 2 do driver
  
  if (serving) {
    motorGarrafa.run();
    motorCopo.run();
  }
  
  if (!motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      digitalWrite(LED_STATUS, HIGH);
      serving = false;
  }
  
  if (!serving && digitalRead(BUTTON)) {
      walk = walk * -1.0;
      motorCopo.moveTo(walk);
      motorGarrafa.moveTo(walk);
      serving = true;
      digitalWrite(LED_STATUS, LOW);
  }
}
