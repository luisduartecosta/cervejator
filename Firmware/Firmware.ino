#include <AccelStepper.h>
#include "Pins.h"

AccelStepper motorCopo(4, 2, 3, 4, 5);
AccelStepper motorGarrafa(4, 6, 7, 8, 9);

int stepCount = 0;
bool serving = false;
int loopsAposInverter = 0;

float walkGarrafa = 20000.0;
float walkCopo = 6000.0;

void beep(int freq, int dur) {
  tone(BUZZER,freq,dur);
}

void setup() {
  pinMode(ENABLE_1_GARRAFA,OUTPUT);
  pinMode(ENABLE_2_GARRAFA,OUTPUT);
  pinMode(LED_STATUS,OUTPUT);
  pinMode(BUTTON, INPUT);


  motorCopo.setMaxSpeed(200.0);
  motorCopo.setAcceleration(60.0);

  motorGarrafa.setMaxSpeed(1000.0);
  motorGarrafa.setAcceleration(60.0);

  serving = true;

  delay(1000);
  digitalWrite(LED_STATUS, LOW);
}

void loop() {
  digitalWrite(ENABLE_1_GARRAFA, HIGH);  // Enable 1 do driver
  digitalWrite(ENABLE_2_GARRAFA, HIGH);  // Enable 2 do driver

  digitalWrite(LED_STATUS, !serving);

  if (serving) {
    motorGarrafa.run();
    motorCopo.run();
  }

  if (!serving && !digitalRead(BTN_1)) {
      motorCopo.move(walkCopo);
      motorGarrafa.move(walkGarrafa);
      serving = true;
  }

  if (!motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      serving = false;
  }

  if (!digitalRead(BTN_2) && !serving && !loopsAposInverter) {
      walkGarrafa = -walkGarrafa;
      walkCopo = -walkCopo;
      loopsAposInverter = 200;
      beep(800,200);
      delay(500);
  }

  if (loopsAposInverter) {
      loopsAposInverter--;
  }
}
