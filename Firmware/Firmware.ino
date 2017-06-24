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
float tempoPosicaoFinal = 0;

int stepCount = 0;
bool running = false;
bool fim = false;
bool chegouAgoraNaPosicaoFinal = false;
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
   running = !running;
   if (running) {
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

  motorCopo.setMaxSpeed(800.0);
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
  #endif
  beep(880,50);
  running = true;
  delay(1000);
  beep(880,50);
  delay(100);
  beep(880,50);
  digitalWrite(LED_GREEN, LOW);
}

void loop() {

  digitalWrite(LED_GREEN, !running);

  limitExcursion(motorCopo, ENDSTOP_GLASS, GLASS_EXCURSION);
  limitExcursion(motorGarrafa, ENDSTOP_BOTTLE, BOTTLE_EXCURSION);

  if (running) {
    motorGarrafa.run();
    motorCopo.run();
  }

  if (!running && !digitalRead(BTN_1) && !serving) {
    // Stella
     serving = true;
     chegouAgoraNaPosicaoFinal = false;
     fim = false;
     beep(200,100);
     delay(100);
     beep(600,100);
     delay(100);
     beep(1200,100);
  }

  if (!motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      running = false;
  }

  if (serving) {
    if (!motorGarrafa.distanceToGo() && !motorGarrafa.currentPosition() && !motorCopo.distanceToGo() && !motorCopo.currentPosition() && !fim) {
      // 1a Etapa
      motorGarrafa.move(15000);
      Serial.println("Subindo garrafa para 15000");
      motorCopo.move(7000);
      Serial.println("Subindo copo para 7000");
    }

    if (!motorCopo.distanceToGo() && motorCopo.currentPosition() == 7000 && motorGarrafa.currentPosition() == 15000) {
      motorCopo.moveTo(0);
      Serial.println("Descendo copo para 0");
    }

    if (!motorGarrafa.distanceToGo() && motorGarrafa.currentPosition() == 15000) {
      motorGarrafa.move(-1000);
      Serial.println("Voltando 1000 na garrafa para segurar a vazao");
    }

    if (!motorGarrafa.distanceToGo() && motorGarrafa.currentPosition() == 14000) {
      motorGarrafa.move(4000);
      Serial.println("Subindo garrafa 4000");
    }

    if (
      motorGarrafa.currentPosition() == 18000
      && motorCopo.currentPosition() == 0
      && !motorGarrafa.distanceToGo()
      && !chegouAgoraNaPosicaoFinal
    )
    {
       chegouAgoraNaPosicaoFinal = true;

       tempoPosicaoFinal = millis();
       Serial.println("Chegou no final, aguardando servir 5s");
    }

    if ( chegouAgoraNaPosicaoFinal && (millis() - tempoPosicaoFinal) > 3000 && !fim)
    {
       motorGarrafa.moveTo(0);
       fim = true;
       Serial.println("Voltando a garrafa para o inicio");
    }

    if (fim && !motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      serving = false;
      beep(200,100);
       delay(100);
       beep(600,100);
       delay(100);
       beep(1200,100);
       Serial.println("Fim!");
    }
    running = true;
  }


  if (!digitalRead(BTN_2) && !running && !loopsAposInverter) {

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
