#ifndef BEEP_H
#define BEEP_H

void beep(int freq, int dur);

void beep(int freq, int dur) {
  tone(BUZZER,freq,dur);
}

#endif