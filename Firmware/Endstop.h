#ifndef ENDSTOP_H
#define ENDSTOP_H

#include <AccelStepper.h>
#include "Beep.h"

enum AxisState {
   UNSET,
   SEEKING_ENDSTOP,
   ENDSTOP_PRESSED,
   BOUNCING,
   READY
};

void resetPosition(AccelStepper &motor, const int endstopPin, const float setupDistance, const float bounceDistance, AxisState &state, AxisState &lastState );

void resetPosition(AccelStepper &motor, const int endstopPin, const float setupDistance, const float bounceDistance, AxisState &state, AxisState &lastState )
{
  lastState = state;
  if ( !digitalRead(endstopPin) ) { // Endstop pressed
    beep(1000,100);
    motor.moveTo( motor.currentPosition() ); // Stop motor
    if ( state != ENDSTOP_PRESSED ) {
        state = ENDSTOP_PRESSED;
    } else {
        state = BOUNCING;
    }
    return;
  }

  if ( state == UNSET ) {
    state = SEEKING_ENDSTOP;
    return;
  }

  if ( state == SEEKING_ENDSTOP ) {
      if ( lastState != SEEKING_ENDSTOP ) {
          beep(1000,150);
          motor.move(setupDistance); // Talvez trocar por #define
      } else {
          motor.run();
      }
  }

  if ( state == BOUNCING ) {
      if ( lastState != BOUNCING ) {
             motor.move(bounceDistance);
      } else {
          if ( motor.distanceToGo() ) {
              motor.run();
          } else {
              motor.setCurrentPosition(0.0f);
              state = READY;
              beep(1200,200);
          }
      }
  }
}
#endif
