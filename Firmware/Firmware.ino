#include <AccelStepper.h>
#define SERIALCOMMANDDEBUG 1
#include <SerialCommand.h>
//#define DEBUG_BEEP
#define DEBUG_SERIAL 1


#include "Pins.h"
#include "Beep.h"
#include "Endstop.h"
#include "Recording.h"

#define BOTTLE_EXCURSION 18000.0
#define GLASS_EXCURSION  7000.0

AccelStepper motorCopo(4, 2, 3, 4, 5);
AccelStepper motorGarrafa(4, 6, 7, 8, 9);

char commandBuffer[20];
int posicaoBuffer = 0;

int stepCount = 0;
bool serving = false;
int loopsAposInverter = 0;

float walkGarrafa = BOTTLE_EXCURSION;
float walkCopo = GLASS_EXCURSION;

#ifdef DEBUG_SERIAL
long glassPositionSnapshot;
long bottlePositionSnapshot;

SerialCommand sCmd;
#endif

AxisState glassState = UNSET;
AxisState lastGlassState = UNSET;
AxisState bottleState = UNSET;
AxisState lastBottleState = UNSET;

void defaultHandler() {beep(200,200); return;}

void cmdMoveMotor(AccelStepper &motor)
{
  char* arg;
  int distance;

  arg = sCmd.next();
  distance = atoi(arg);
  if ( abs(motor.currentPosition() - motor.targetPosition()) < 200 )
  {
    motor.move(distance);
  }
}

void printInstantPosition() {
  Serial.print("GLASS:");
  Serial.print(glassState, DEC);
  Serial.print(" Glass position:");
  Serial.print(motorCopo.currentPosition(),DEC);
  Serial.print(" - BOTTLE:");
  Serial.print(bottleState, DEC);
  Serial.print(" Bottle position:");
  Serial.println(motorGarrafa.currentPosition(),DEC);
}

void serve() {
   serving = !serving;
   if (serving) {
      beep(800,100);
      beep(1600,100);
   }
}

void storePosition() {
  if ( regIndex < MAX_REG ) {
       posicaoCopo[regIndex] = motorCopo.currentPosition();
       posicaoGarrafa[regIndex] = motorGarrafa.currentPosition();
       tempo[regIndex] = millis();
       regIndex++;
       beep(800,200);
  } else {
       beep(200,800);
  }
}

void moveGlass() {
   cmdMoveMotor(motorCopo);
}

void moveBottle() {
  cmdMoveMotor(motorGarrafa);
}

void stopAll() {
  beep(400,100);
  motorGarrafa.moveTo(motorGarrafa.currentPosition());
  motorCopo.moveTo(motorGarrafa.currentPosition());
}

void resetList() {
  posicaoCopo[0] = motorCopo.currentPosition();
  posicaoGarrafa[0] = motorGarrafa.currentPosition();
  tempo[0] = millis();
  regIndex = 1;
  beep(1000,100);
}

void printPositions() {
    beep(800,100);
    beep(1000,100);
    for (int envioIndex = 0; envioIndex < regIndex; envioIndex++) {
       Serial.println("tempo,garrafa,copo");
       Serial.print(posicaoCopo[envioIndex], DEC);
       Serial.print(",");
       Serial.print(posicaoGarrafa[envioIndex], DEC);
       Serial.print(",");
       Serial.println(tempo[envioIndex], DEC);
    }
}

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
    resetPosition(motorCopo,ENDSTOP_GLASS,-(GLASS_EXCURSION + 1000),500,glassState,lastGlassState);
    resetPosition(motorGarrafa,ENDSTOP_BOTTLE,-(BOTTLE_EXCURSION + 4000),500,bottleState,lastBottleState);
  }
  digitalWrite(LED_RED, (glassState == READY));
  digitalWrite(LED_GREEN, (bottleState == READY));

  #ifdef DEBUG_SERIAL
  sCmd.addCommand("POS", printInstantPosition);
  sCmd.addCommand("REC", storePosition);
  sCmd.addCommand("MVCOPO", moveGlass);
  sCmd.addCommand("MVGAR", moveBottle);
  sCmd.addCommand("STOP", stopAll);
  sCmd.addCommand("LIST", printPositions);
  sCmd.addCommand("GO", serve);
  sCmd.addDefaultHandler(defaultHandler);

  printInstantPosition();
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

  }

  if (!motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      serving = false;
  }


  if (!digitalRead(BTN_2) && !serving && !loopsAposInverter) {

  }

  if (loopsAposInverter) {
      loopsAposInverter--;
  }
  
  sCmd.readSerial();
  
//  if (Serial.available() > 0) {
//     commandBuffer[posicaoBuffer] = Serial.read();
//     
//     if ( commandBuffer[posicaoBuffer] == '\r' ) {
//         commandBuffer[posicaoBuffer] = '\0';
//         Serial.println("->");
//         Serial.println(commandBuffer);
//         posicaoBuffer = 0;
//     } else {
//         posicaoBuffer++;
//     }     
//  }
}
