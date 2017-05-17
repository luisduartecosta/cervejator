#include <Stepper.h>

const int stepsPerRevolution = 200; 
Stepper motorCopo(stepsPerRevolution, 2, 3, 4, 5);   
Stepper motorGarrafa(stepsPerRevolution, 6, 7, 8, 9); 

int stepCount = 0;

void setup() {
  // initialize the serial port:
  motorCopo.setSpeed(200);
  motorGarrafa.setSpeed(200);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  digitalWrite(11, HIGH);  // Enable 1 do driver
  digitalWrite(10, HIGH);  // Enable 2 do driver
  // step one step:
  //motorCopo.step(40);
  delay(10);
  motorGarrafa.step(-80);
  Serial.print("steps:" );
  Serial.println(stepCount);
  stepCount++;
  delay(30);
}
