#ifndef MAIN_C
#define MAIN_C

#include "challenges.c"
#include "globals.h"
#include "io.c"

int main() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            map[i][j] = -1;
        }
    }

    IO_initializeTOFs();

    /*
    // set startDirection to CLOCKWISE if left TOF sees edge of track, COUNTERCLOCKWISE if right TOF sees it instead
    float leftDistance;
    float rightDistance;
    do {
        leftDistance = IO_readTOF(HORIZONTAL, LEFT);
        rightDistance = IO_readTOF(HORIZONTAL, RIGHT);
        startDirection = rightDistance <= SEE_WALL_DISTANCE && leftDistance >= SEE_WALL_DISTANCE;
    } while (leftDistance > SEE_WALL_DISTANCE && rightDistance > SEE_WALL_DISTANCE);
     */
    startDirection = CLOCKWISE;

    // calibrate sensors
    BNO_init_i2cbus();

    IO_initializeDriveMotor();
    IO_initializeSteeringMotor();
    IO_writeToDriveMotor(100);
    IO_writeToSteeringMotor(0, FORWARDS);

    // driveOpenChallenge();
    //  driveObstacleChallenge();

    // driveFirstLapStretch(startDirection);

    float frontDistance;
    int direction = CLOCKWISE;

    do {
        frontDistance = IO_readTOF(VERTICAL, direction);
    } while (frontDistance > TURN_DISTANCE / 3);

    float orientation;

    IO_writeToSteeringMotor(OPEN_TURN_ANGLE, direction);

    // Turn until turn completed
    do {
        orientation = IO_readGyroscope(startDirection, direction, 1);
    } while (orientation < 90);

    IO_writeToSteeringMotor(STRAIGHT_STEERING, direction);

    printf("stopping");

    IO_writeToDriveMotor(0);
    sleep(2);
    IO_writeToSteeringMotor(0, FORWARDS);
    sleep(2);
    IO_uninitializeDriveMotor();
    IO_uninitializeSteeringMotor();

    printf("\nthe end.\n");

    return 0;
}

#endif // MAIN_C