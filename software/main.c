#include "challenges.c"
#include "globals.h"
#include "io.c"

int main() {
    // set startDirection to CLOCKWISE if left TOF sees edge of track, COUNTERCLOCKWISE if right TOF sees it instead
    float leftDistance;
    float rightDistance;
    do {
        leftDistance = IO_readTOF(HORIZONTAL, LEFT);
        rightDistance = IO_readTOF(HORIZONTAL, RIGHT);
        startDirection = leftDistance <= SEE_WALL_DISTANCE && rightDistance >= SEE_WALL_DISTANCE;
    } while (leftDistance > SEE_WALL_DISTANCE && rightDistance > SEE_WALL_DISTANCE);

    // calibrate sensors

    // driveOpenChallenge();
    // driveObstacleChallenge();

    return 0;
}