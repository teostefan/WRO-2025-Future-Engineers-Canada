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
    struct bnoeul testAngles = {0.0, 0.0, 0.0};
    BNO_init_i2cbus();
    if (BNO_get_eul(&testAngles, startDirection, startDirection, 1) == -1) {
        printf("An error occurred.");
    }

    IO_initializeDriveMotor();
    IO_initializeSteeringMotor();
    IO_writeToSteeringMotor(0, FORWARDS);

    // driveOpenChallenge();
    // driveObstacleChallenge();

    IO_uninitializeDriveMotor();
    IO_writeToSteeringMotor(0, FORWARDS);
    IO_uninitializeSteeringMotor();

    printf("\nthe end.\n");

    return 0;
}