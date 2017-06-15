#include <AccelStepper.h>

//#define DEBUG_BEEP
//#define DEBUG_SERIAL


#include "Pins.h"
#include "Beep.h"
#include "Endstop.h"

#define BOTTLE_EXCURSION 15500.0
#define GLASS_EXCURSION  5000.0

AccelStepper motorCopo(4, 2, 3, 4, 5);
AccelStepper motorGarrafa(4, 6, 7, 8, 9);

int stepCount = 0;
bool serving = false;
int loopsAposInverter = 0;

float walkGarrafa = BOTTLE_EXCURSION;
float walkCopo = GLASS_EXCURSION;

#ifdef DEBUG_SERIAL
long glassPositionSnapshot;
long bottlePositionSnapshot;
#endif

AxisState glassState = UNSET;
AxisState lastGlassState = UNSET;
AxisState bottleState = UNSET;
AxisState lastBottleState = UNSET;

void setup() {
  #ifdef DEBUG_SERIAL
  Serial.begin(115200);
  #endif
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
  #ifdef DEBUG_SERIAL
  Serial.println("Press btn 1 to continue to endstop routine");
  while (digitalRead(BTN_1)) {};
  #endif
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);
  while ( glassState != READY || bottleState != READY )
  {
    resetPosition(motorCopo,ENDSTOP_GLASS,-(GLASS_EXCURSION),500,glassState,lastGlassState);
    resetPosition(motorGarrafa,ENDSTOP_BOTTLE,-(BOTTLE_EXCURSION + 4000),500,bottleState,lastBottleState);
  }
  digitalWrite(LED_RED, (glassState == READY));
  digitalWrite(LED_GREEN, (bottleState == READY));

  #ifdef DEBUG_SERIAL
  Serial.print("GLASS:");
  Serial.print(glassState, DEC);
  Serial.print(" Glass position:");
  Serial.print(motorCopo.currentPosition(),DEC);
  Serial.print(" - BOTTLE:");
  Serial.print(bottleState, DEC);
  Serial.print(" Bottle position:");
  Serial.print(motorGarrafa.currentPosition(),DEC);
  Serial.println("Endstops ready, press btn 1 to continue");
  while (digitalRead(BTN_1)) {};
  #endif
  beep(880,50);
  serving = true;
  delay(1000);
  beep(880,50);
  delay(100);
  beep(880,50);
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
      delay(100);
      return;
  }

  #ifdef DEBUG_SERIAL
  if (serving && !digitalRead(BTN_2) ) {
      motorCopo.moveTo(motorCopo.currentPosition());
      glassPositionSnapshot = motorCopo.currentPosition();
  }

  if (serving && !digitalRead(BTN_1) ) {
      motorGarrafa.moveTo(motorGarrafa.currentPosition());
      bottlePositionSnapshot = motorGarrafa.currentPosition();
  }
  #endif

  if (!motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      serving = false;
      #if DEBUG_SERIAL
      Serial.print("Glass position:");
      Serial.print(glassPositionSnapshot,DEC);
      Serial.print(" Bottle position:");
      Serial.print(bottlePositionSnapshot,DEC);
      Serial.println("Paused");
      #endif
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
