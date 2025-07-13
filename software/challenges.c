
#ifndef CHALLENGES_C
#define CHALLENGES_C

#include "globals.h"
#include "laps.c"

int driveObstacleChallenge() {
    pullOut();
    driveFirstLap(startDirection);
    turnAround(startDirection);
    driveNonFirstLap(!startDirection);
    turnAround(!startDirection);
    driveNonFirstLap(startDirection);
    parallelPark();

    return 0;
}

int driveOpenChallenge() {
    // pullOut();
    driveOpenLap(startDirection);
    turnAround(startDirection);
    driveOpenLap(!startDirection);
    turnAround(!startDirection);
    driveOpenLap(startDirection);
    parallelPark();

    return 0;
}

#endif // CHALLENGES_C