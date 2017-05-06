#include <Stepper.h>

const int stepsPerRevolution = 200; 
Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5); // TODO - Mudar os nomes
Stepper myStepper2(stepsPerRevolution, 6, 7, 8, 9); 

int stepCount = 0;

void setup() {
  // initialize the serial port:
  myStepper.setSpeed(200);
  myStepper2.setSpeed(200);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  digitalWrite(11, HIGH);  // Enable 1 do driver
  digitalWrite(10, HIGH);  // Enable 2 do driver
  // step one step:
  myStepper.step(1);
  myStepper2.step(1);
  Serial.print("steps:" );
  Serial.println(stepCount);
  stepCount++;
  delay(100);
}
