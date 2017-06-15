#ifndef PS2X_H
#define PS2X_H

#include <AccelStepper.h>

#define PS2_DAT          14   // Marrom do PS  --  Laranja
#define PS2_CMD          15   // Laranja do PS -- Roxo
#define PS2_ATT          13   // Amarelo do PS -- Cinza
#define PS2_CLK          12   // Azul do PS    -- Preto

#define MIN_DEADZONE     118
#define MAX_DEADZONE     138

int erroControle = 0;

byte leituraControle;

void controlarMotor(const int leituraControle, AccelStepper &motor );

void controlarMotor(const int leituraControle, AccelStepper &motor) {
   float walk;
    if ( abs(motor.currentPosition() - motor.targetPosition()) < 200 ) {
        // Zona morta do controle
        if ( !(leituraControle > MIN_DEADZONE && leituraControle < MAX_DEADZONE) ) {
            walk = map(leituraControle, 0, 255, -200, 200);
            motor.move(walk);
        }
    }

    if (motor.targetPosition() < 0 ) {
        motor.moveTo(0);
    }
};

#endif