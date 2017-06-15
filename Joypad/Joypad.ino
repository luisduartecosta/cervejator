#define PS2X_DEBUG
#include <PS2X_lib.h>
#include <AccelStepper.h>

//#define DEBUG_BEEP
#define DEBUG_SERIAL

#include "Pins.h"
#include "PS2X.h"
#include "Beep.h"
#include "Endstop.h"
#include "Recording.h"

#define BOTTLE_EXCURSION 18500.0
#define GLASS_EXCURSION  5000.0

AccelStepper motorCopo(4, 2, 3, 4, 5);
AccelStepper motorGarrafa(4, 6, 7, 8, 9);

PS2X Joypad;

int stepCount = 0;
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

  Serial.begin(115200);

  erroControle = Joypad.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, false, false);
  
  delay(100);
  
  if (!erroControle) {
      beep(1000,100);
  } else {
      beep(200,1000);
      Serial.print("PS2 Controller error");
      Serial.println(erroControle, DEC);
      return;
  }

  delay(500);

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
  delay(500);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, LOW);

  while ( glassState != READY || bottleState != READY )
  {
    resetPosition(motorCopo,ENDSTOP_GLASS,-walkCopo,500,glassState,lastGlassState);
    resetPosition(motorGarrafa,ENDSTOP_BOTTLE,-walkGarrafa,500,bottleState,lastBottleState);
  }
  digitalWrite(LED_RED, (glassState == READY));
  digitalWrite(LED_GREEN, (bottleState == READY));

  Serial.println("Endstop setup done");

  beep(880,50);
  delay(1000);
  digitalWrite(LED_GREEN, LOW);
}

void loop() {
  if (erroControle) return;

  Joypad.read_gamepad();

  controlarMotor(Joypad.Analog(PSS_LY), motorCopo);
  controlarMotor(Joypad.Analog(PSS_RY), motorGarrafa);
  // Esses limites de excursão dependendem um da posição do outro...
  limitExcursion(motorCopo, ENDSTOP_GLASS, GLASS_EXCURSION);
  limitExcursion(motorGarrafa, ENDSTOP_BOTTLE, BOTTLE_EXCURSION);
  motorGarrafa.run();
  motorCopo.run();

  // Iniciar gravacao
  if ( Joypad.ButtonPressed(PSB_CROSS) ) {
       posicaoCopo[0] = motorCopo.currentPosition();
       posicaoGarrafa[0] = motorGarrafa.currentPosition();
       tempo[0] = millis();
       regIndex = 1;
       beep(800,100);
  }

   // Armazenar valor
  if ( Joypad.ButtonPressed(PSB_L1) ) {
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

  // Mandar gravacao
  if ( Joypad.ButtonPressed(PSB_TRIANGLE) ) {
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

  if (!motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      digitalWrite(LED_GREEN, HIGH);
  } else {
      digitalWrite(LED_GREEN, LOW);
  }
}
