#include <PS2X_lib.h>
#include <AccelStepper.h>

#define ENABLE_1_GARRAFA 10
#define ENABLE_2_GARRAFA 11
#define LED_STATUS       51
#define BUTTON           53
#define BUZZER           37
#define BTN_1            39
#define BTN_2            41
#define BTN_3            43
#define ENDSTOP_1        25
#define ENDSTOP_2        29

#define PS2_DAT          13
#define PS2_CMD          15
#define PS2_SEL          14
#define PS2_CLK          12

#define MAX_GARRAFA      20000.0
#define MAX_COPO         6000.0

#define MAX_REG          20

AccelStepper motorCopo(4, 2, 3, 4, 5);   
AccelStepper motorGarrafa(4, 6, 7, 8, 9); 

PS2X Joypad;

int stepCount = 0;
bool serving = false;
int loopsAposInverter = 0;

bool copoNoZero = false;
bool garrafaNoZero = false;

float walkGarrafa = MAX_GARRAFA;
float walkCopo = MAX_COPO;


long posicaoCopo[MAX_REG];
long posicaoGarrafa[MAX_REG];
long tempo[MAX_REG];

int regIndex = 0;
int envioIndex = 0;
int erroControle = 0;

byte leituraControle;

void setup() {
  pinMode(ENABLE_1_GARRAFA,OUTPUT);
  pinMode(ENABLE_2_GARRAFA,OUTPUT);
  pinMode(LED_STATUS,OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(ENDSTOP_1,INPUT);
  pinMode(ENDSTOP_2,INPUT);
  
  Serial.begin(115200);
  
  erroControle = Joypad.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, false, false);
  
  if (!erroControle) {
     tone(BUZZER,1000,150);
  } else {
     tone(BUZZER,500,800);
     return;
  }

  //motorCopo.setSpeed(300);
  //motorGarrafa.setSpeed(300);

  motorCopo.setMaxSpeed(200.0);
  motorCopo.setAcceleration(60.0);

  motorGarrafa.setMaxSpeed(1000.0);
  motorGarrafa.setAcceleration(60.0);

  serving = true;

  delay(1000);
  //Serial.begin(9600);
  digitalWrite(LED_STATUS, LOW);

  copoNoZero = digitalRead(ENDSTOP_1);
  garrafaNoZero = digitalRead(ENDSTOP_2);

  if ( !copoNoZero || !garrafaNoZero ) {
     motorCopo.move(walkCopo);
     motorGarrafa.move(walkGarrafa);
     do {
         if (!copoNoZero) {
           motorCopo.run();
         }

         if (!garrafaNoZero) {
           motorGarrafa.run();
         }

         copoNoZero = digitalRead(ENDSTOP_1);
         garrafaNoZero = digitalRead(ENDSTOP_2);

      } while (!copoNoZero || !garrafaNoZero);
   }

   motorCopo.setCurrentPosition(0.0f);
   motorGarrafa.setCurrentPosition(0.0f);

   walkCopo = 6000.0;
   walkGarrafa = 20000.0;
}

void loop() {
  if (erroControle) return; 
  
  digitalWrite(ENABLE_1_GARRAFA, HIGH);  // Enable 1 do driver
  digitalWrite(ENABLE_2_GARRAFA, HIGH);  // Enable 2 do driver

  digitalWrite(LED_STATUS, !serving);
  
  
  Joypad.read_gamepad();
  
  if ( abs(motorCopo.currentPosition() - motorCopo.targetPosition()) < 200 ) {
    leituraControle = Joypad.Analog(PSS_LY);
    // Zona morta do controle
    if ( !(leituraControle > 100 && leituraControle < 150) ) {    
        walkCopo = map(leituraControle, 0, 255, -200, 200);
        if ( motorCopo.currentPosition() + walkCopo < MAX_COPO ) {
           motorCopo.move(walkCopo);
        }
     } 
  }
  
  if (motorCopo.targetPosition() < 0 ) {
     motorCopo.moveTo(0);
  }
  
  motorGarrafa.run();
  motorCopo.run();
  
  if ( abs(motorGarrafa.currentPosition() - motorGarrafa.targetPosition()) < 200 ) {
    leituraControle = Joypad.Analog(PSS_RY);
    // Zona morta do controle
    if ( !(leituraControle > 100 && leituraControle < 150) ) {    
        walkGarrafa = map(leituraControle, 0, 255, -200, 200);
        if ( (motorGarrafa.currentPosition() + walkGarrafa < MAX_COPO) ) {
           motorGarrafa.move(walkGarrafa);
        }
     } 
  }
  
  if (motorGarrafa.targetPosition() < 0 ) {
     motorGarrafa.moveTo(0);
  }
 
  motorGarrafa.run();
  motorCopo.run();
  
  // Iniciar gravacao
  if ( Joypad.ButtonPressed(PSB_CROSS) ) {
       posicaoCopo[0] = motorCopo.currentPosition();
       posicaoGarrafa[0] = motorGarrafa.currentPosition();
       tempo[0] = millis();
       regIndex = 1;
       tone(BUZZER,800,100);
  }
  
  // Mandar gravacao
  if ( Joypad.ButtonPressed(PSB_TRIANGLE) ) {
    for (int envioIndex = 0; envioIndex < regIndex; envioIndex++) {
       Serial.println("tempo,garrafa,copo");
       Serial.print(posicaoCopo[envioIndex], DEC);
       Serial.print(",");
       Serial.print(posicaoGarrafa[envioIndex], DEC);
       Serial.print(",");
       Serial.print(tempo[envioIndex], DEC);
       Serial.print("\r\n");
    }
  }
  
  // Armazenar valor
  if ( Joypad.ButtonPressed(PSB_L1) ) {
     if ( regIndex < MAX_REG ) {
       posicaoCopo[regIndex] = motorCopo.currentPosition();
       posicaoGarrafa[regIndex] = motorGarrafa.currentPosition();
       tempo[regIndex] = millis();
       regIndex++;
       tone(BUZZER,800,200);
     } else {
       tone(BUZZER,200,800);
     }
  }

  // Depois de um tempo (em que o copo esteja quase cheio)tem que mandar descer os dois (o copo até o final, e a garrafa um pouco)
  // Termina de servir a cerveja com o copo na posição inical pra fazer o colarinho e manda a garrafa pra posição inical
  
  
}
