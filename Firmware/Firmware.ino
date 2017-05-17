#include <AccelStepper.h>

#define ENABLE_1_GARRAFA 10
#define ENABLE_2_GARRAFA 11
#define LED_STATUS       53

AccelStepper motorCopo(4, 2, 3, 4, 5);   
AccelStepper motorGarrafa(4, 6, 7, 8, 9); 

int stepCount = 0;

void setup() {
  pinMode(ENABLE_1_GARRAFA,OUTPUT);
  pinMode(ENABLE_2_GARRAFA,OUTPUT);
  pinMode(LED_STATUS,OUTPUT);
  
  motorCopo.setSpeed(200);
  motorGarrafa.setSpeed(200);
  
  motorCopo.setMaxSpeed(200.0);
  motorCopo.setAcceleration(30.0);
  motorCopo.moveTo(-2000);
  
  motorGarrafa.setMaxSpeed(200.0);
  motorGarrafa.setAcceleration(30.0);
  motorGarrafa.moveTo(-2000);

  delay(1000);
  
  digitalWrite(LED_STATUS, LOW);
}

void loop() {
  digitalWrite(ENABLE_1_GARRAFA, HIGH);  // Enable 1 do driver
  digitalWrite(ENABLE_2_GARRAFA, HIGH);  // Enable 2 do driver
  
  motorGarrafa.run();
  motorCopo.run();
  
  if (!motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      digitalWrite(LED_STATUS, HIGH);
  }
}
