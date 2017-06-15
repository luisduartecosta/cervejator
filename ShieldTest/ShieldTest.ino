#include "Pins.h"

void beep(int freq, int dur) {
  tone(BUZZER,freq,dur);
}

void setup() {
  pinMode(ENABLE_1_GARRAFA,OUTPUT);
  pinMode(ENABLE_2_GARRAFA,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_RED,OUTPUT);

  digitalWrite(ENABLE_1_GARRAFA, LOW);
  digitalWrite(ENABLE_2_GARRAFA, LOW);
  digitalWrite(LED_GREEN, LOW);
  delay(200);
  digitalWrite(LED_GREEN, HIGH);
  delay(200);
  digitalWrite(LED_GREEN, LOW);
  delay(200);
  digitalWrite(LED_GREEN, HIGH);
  delay(200);
  digitalWrite(LED_RED, LOW);
  delay(200);
  digitalWrite(LED_RED, HIGH);
  delay(200);
  digitalWrite(LED_RED, LOW);
  delay(200);
  digitalWrite(LED_RED, HIGH);
  delay(200);

  Serial.begin(115200);
}

void loop() {

  digitalWrite(LED_GREEN, !digitalRead(BTN_1));
  digitalWrite(LED_RED, !digitalRead(BTN_2));

  Serial.print("BTN_1:");
  Serial.print(!digitalRead(BTN_1), DEC);
  Serial.print(" BTN_2:");
  Serial.print(!digitalRead(BTN_2), DEC);
  Serial.print(" ENDSTOP_GLASS:");
  Serial.print(!digitalRead(ENDSTOP_GLASS), DEC);
  Serial.print(" ENDSTOP_BOTTLE:");
  Serial.print(!digitalRead(ENDSTOP_BOTTLE), DEC);
  Serial.print("\r\n");

  if (!digitalRead(BTN_1)) {
     beep(440,100);
  }

  if (!digitalRead(BTN_2)) {
     beep(640,100);
  }

  if (!digitalRead(ENDSTOP_GLASS)) {
    beep(800,100);
  }

  if (!digitalRead(ENDSTOP_BOTTLE)) {
    beep(1000,100);
  }

}
