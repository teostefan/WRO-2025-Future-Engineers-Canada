#ifndef LAPS_C
#define LAPS_C

#include "globals.h"
#include "manoeuvres.c"

int driveFirstLap(int direction) {
    // Drive through LAP_TURN_NUMBER turn and straight sections
    for (int i = 0; i < LAP_TURN_NUMBER; i++) {
        if (direction == startDirection) {
            // Laps 1 and 3 begin with a straight section and end with a turn section
            stretch = i;
            driveFirstLapStretch(direction);
            driveFirstLapTurn(direction);
        } else {
            // Lap 2 begins with a turn section and ends with a straight section
            stretch = LAP_TURN_NUMBER - i - 1;
            driveFirstLapTurn(direction);
            driveFirstLapStretch(direction);
        }
    }

    stretch = 3 - stretch;

    return 0;
}

int driveNonFirstLap(int direction) {
    // Drive through LAP_TURN_NUMBER turn and straight sections
    for (int i = 0; i < LAP_TURN_NUMBER; i++) {
        if (direction == startDirection) {
            // Laps 1 and 3 begin with a straight section and end with a turn section
            stretch = i;
            driveStretch(direction);
            driveTurn(direction);
        } else {
            // Lap 2 begins with a turn section and ends with a straight section
            stretch = LAP_TURN_NUMBER - i - 1;
            driveTurn(direction);
            driveStretch(direction);
        }
    }

    stretch = 3 - stretch;

    return 0;
}

int driveOpenLap(int direction) {
    // Drive through LAP_TURN_NUMBER turn and straight sections
    for (int i = 0; i < LAP_TURN_NUMBER; i++) {
        if (direction == startDirection) {
            // Laps 1 and 3 begin with a straight section and end with a turn section
            stretch = i;
            driveOpenStretch(direction);
            driveOpenTurn(direction);
        } else {
            // Lap 2 begins with a turn section and ends with a straight section
            stretch = LAP_TURN_NUMBER - i - 1;
            driveOpenTurn(direction);
            driveOpenStretch(direction);
        }
    }

    stretch = 3 - stretch;

    return 0;
}

#endif // LAPS_C