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

AccelStepper motorCopo(4, 2, 3, 4, 5);   
AccelStepper motorGarrafa(4, 6, 7, 8, 9); 

int stepCount = 0;
bool serving = false;
int loopsAposInverter = 0;

bool copoNoZero = false;
bool garrafaNoZero = false;

float walkGarrafa = -20000.0;
float walkCopo = -6000.0;

void setup() {
  pinMode(ENABLE_1_GARRAFA,OUTPUT);
  pinMode(ENABLE_2_GARRAFA,OUTPUT);
  pinMode(LED_STATUS,OUTPUT);
  pinMode(BUTTON, INPUT);
  pinMode(ENDSTOP_1,INPUT);
  pinMode(ENDSTOP_2,INPUT);

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
  digitalWrite(ENABLE_1_GARRAFA, HIGH);  // Enable 1 do driver
  digitalWrite(ENABLE_2_GARRAFA, HIGH);  // Enable 2 do driver

  digitalWrite(LED_STATUS, !serving);
//Começo do serviço

  if (serving) {
     motorGarrafa.run();
     motorCopo.run();
     //Serial.write("servindo... -> Copo:\n");
  }

  if (!serving && !digitalRead(BTN_3)) {
     motorCopo.move(walkCopo);
     motorGarrafa.move(walkGarrafa);
     serving = true;
     //Serial.write("Ordem de servir :\n");
  }

  if (!motorGarrafa.distanceToGo() && !motorCopo.distanceToGo()) {
      serving = false;
      //Serial.write("esperando...\n");
  }

  // Depois de um tempo (em que o copo esteja quase cheio)tem que mandar descer os dois (o copo até o final, e a garrafa um pouco)
  // Termina de servir a cerveja com o copo na posição inical pra fazer o colarinho e manda a garrafa pra posição inical

  if (!digitalRead(BTN_2) && !serving && !loopsAposInverter) {
      walkGarrafa = -walkGarrafa;
      walkCopo = -walkCopo;
      //Serial.write("Inverteu\n");
      loopsAposInverter = 200;
      tone(BUZZER,800,200);
      delay(500);
  }
  
  if (loopsAposInverter) {
      loopsAposInverter--;
  }
}
