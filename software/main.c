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

    camera = CV_getcamera("/dev/video0", "gblur=0.5");
    if (!camera) cameraData.obstacle_spotted = 0;

    // set startDirection to CLOCKWISE if left TOF sees edge of track, COUNTERCLOCKWISE if right TOF sees it instead
    float leftDistance;
    float rightDistance;
    do {
        leftDistance = IO_readTOF(HORIZONTAL, LEFT);
        rightDistance = IO_readTOF(HORIZONTAL, RIGHT);
        startDirection = rightDistance <= SEE_WALL_DISTANCE && leftDistance >= SEE_WALL_DISTANCE;
    } while (leftDistance > SEE_WALL_DISTANCE && rightDistance > SEE_WALL_DISTANCE);

    // calibrate sensors
    struct bnoeul testAngles = {0.0, 0.0, 0.0};
    BNO_init_i2cbus();
    if (BNO_get_eul(&testAngles, startDirection, startDirection, 1) == -1) {
        printf("An error occurred.");
    }

    IO_initializeDriveMotor();
    IO_initializeSteeringMotor();
    IO_writeToDriveMotor(0);
    IO_writeToSteeringMotor(0, FORWARDS);

    // driveOpenChallenge();
    // driveObstacleChallenge();

    driveFirstLapStretch(startDirection);

    printf("stopping");

    IO_writeToDriveMotor(0);
    IO_writeToSteeringMotor(0, FORWARDS);
    IO_uninitializeDriveMotor();
    IO_uninitializeSteeringMotor();

    printf("\nthe end.\n");

    return 0;
}