#include <AccelStepper.h>

#include "Pins.h"
#include "Beep.h"
#include "Endstop.h"

#define BOTTLE_EXCURSION 19000.0
#define GLASS_EXCURSION  5000.0

AccelStepper motorCopo(4, 2, 3, 4, 5);
AccelStepper motorGarrafa(4, 6, 7, 8, 9);

int stepCount = 0;
bool serving = false;
int loopsAposInverter = 0;

float walkGarrafa = 20000.0;
float walkCopo = 6000.0;

AxisState glassState = UNSET;
AxisState lastGlassState = UNSET;
AxisState bottleState = UNSET;
AxisState lastBottleState = UNSET;

void setup() {
  pinMode(ENABLE_1_GARRAFA,OUTPUT);
  pinMode(ENABLE_2_GARRAFA,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);

  motorCopo.setMaxSpeed(200.0);
  motorCopo.setAcceleration(60.0);

  motorGarrafa.setMaxSpeed(1000.0);
  motorGarrafa.setAcceleration(60.0);

  digitalWrite(ENABLE_1_GARRAFA, HIGH);
  digitalWrite(ENABLE_2_GARRAFA, HIGH);

  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, HIGH);
  while ( glassState != READY || bottleState != READY )
  {
    resetPosition(motorCopo,ENDSTOP_GLASS,-walkCopo,500,glassState,lastGlassState);
    resetPosition(motorGarrafa,ENDSTOP_BOTTLE,-walkGarrafa,500,bottleState,lastBottleState);
  }
  serving = true;
  delay(1000);
  digitalWrite(LED_GREEN, LOW);
}

void loop() {

  digitalWrite(LED_GREEN, !serving);

  limitExcursion(motorCopo, ENDSTOP_GLASS, GLASS_EXCURSION);
  limitExcursion(motorGarrafa, ENDSTOP_BOTTLE, BOTTLE_EXCURSION);

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
