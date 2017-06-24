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
void limitExcursion(AccelStepper &motor, const int endstopPin, const float maxPosition);

void resetPosition(AccelStepper &motor, const int endstopPin, const float setupDistance, const float bounceDistance, AxisState &state, AxisState &lastState )
{

  if ( state == READY ) return;
  if ( !digitalRead(endstopPin) && state != BOUNCING ) { // Endstop pressed
    #ifdef DEBUG_BEEP
    beep(1000,100);
    #endif
    lastState = state;
    motor.moveTo( motor.currentPosition() ); // Stop motor
    if ( state != ENDSTOP_PRESSED ) {
        state = ENDSTOP_PRESSED;
    } else {
        state = BOUNCING;
    }
    return;
  }

  if ( state == UNSET ) {
    lastState = state;
    state = SEEKING_ENDSTOP;
    return;
  }

  if ( state == SEEKING_ENDSTOP ) {
      if ( lastState != SEEKING_ENDSTOP ) {
          #ifdef DEBUG_BEEP
          beep(1000,50);
          #endif
          motor.moveTo(setupDistance); // Talvez trocar por #define
          lastState = state;
          return;
      } else {
          motor.run();
      }
  }

  if ( state == BOUNCING ) {
      if ( lastState != BOUNCING ) {
             motor.move(bounceDistance);
             lastState = state;
             return;
      } else {
          if ( motor.distanceToGo() ) {
              motor.run();
          } else {
              motor.setCurrentPosition(0.0f);
              motor.moveTo(0.0f);
              state = READY;
              #ifdef DEBUG_BEEP
              beep(1200,200);
              #endif
          }
      }
  }
}

void limitExcursion(AccelStepper &motor, const int endstopPin, const float maxPosition)
{
    if (motor.currentPosition() < 0 )
    {
        motor.moveTo(0.0f);
    }

    if (motor.targetPosition() > maxPosition )
    {
        motor.moveTo(maxPosition);
        beep(640,100);
    }

    if (!digitalRead(endstopPin))
    {
        motor.moveTo(motor.currentPosition());
        beep(440,200);
    }
}
#endif
