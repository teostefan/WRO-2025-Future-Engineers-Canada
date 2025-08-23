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

    float test_leftDistance = IO_readTOF(HORIZONTAL, LEFT);
    printf("\n\nLeft distance: %f", test_leftDistance);
    float test_rightDistance = IO_readTOF(HORIZONTAL, RIGHT);
    printf("\nRight distance: %f", test_rightDistance);
    float test_frontDistance = IO_readTOF(VERTICAL, FRONT);
    printf("\nFront distance: %f", test_frontDistance);
    float test_rearDistance = IO_readTOF(VERTICAL, REAR);
    printf("\nRear distance: %f", test_rearDistance);

    float test_orientation = IO_readGyroscope(startDirection, startDirection, 1);
    printf("\n\nClockwise stretch 1 orientation: %f", test_orientation);
    test_orientation = IO_readGyroscope(startDirection, startDirection, 2);
    printf("\nClockwise stretch 2 orientation: %f", test_orientation);
    test_orientation = IO_readGyroscope(startDirection, startDirection, 3);
    printf("\nClockwise stretch 3 orientation: %f", test_orientation);
    test_orientation = IO_readGyroscope(startDirection, startDirection, 4);
    printf("\nClockwise stretch 4 orientation: %f", test_orientation);
    test_orientation = IO_readGyroscope(startDirection, !startDirection, 1);
    printf("\nCounterclockwise stretch 1 orientation: %f", test_orientation);
    test_orientation = IO_readGyroscope(startDirection, !startDirection, 2);
    printf("\nCounterclockwise stretch 2 orientation: %f", test_orientation);
    test_orientation = IO_readGyroscope(startDirection, !startDirection, 3);
    printf("\nCounterclockwise stretch 3 orientation: %f", test_orientation);
    test_orientation = IO_readGyroscope(startDirection, !startDirection, 4);
    printf("\nCounterclockwise stretch 4 orientation: %f", test_orientation);

    sleep(2);

    struct CV_CameraData test_cameraVision = IO_readCamera();
    printf("\n\nobstacle spotted: %d", test_cameraVision.obstacle_spotted);
    printf("\nobstacle x: %d", test_cameraVision.obstacle_x);
    printf("\nobstacle y: %d", test_cameraVision.obstacle_y);
    printf("\nobstacle colour: %d", test_cameraVision.obstacle_colour);

    sleep(2);

    test_cameraVision = IO_readCamera();
    printf("\n\nobstacle spotted: %d", test_cameraVision.obstacle_spotted);
    printf("\nobstacle x: %d", test_cameraVision.obstacle_x);
    printf("\nobstacle y: %d", test_cameraVision.obstacle_y);
    printf("\nobstacle colour: %d", test_cameraVision.obstacle_colour);

    sleep(2);

    test_cameraVision = IO_readCamera();
    printf("\n\nobstacle spotted: %d", test_cameraVision.obstacle_spotted);
    printf("\nobstacle x: %d", test_cameraVision.obstacle_x);
    printf("\nobstacle y: %d", test_cameraVision.obstacle_y);
    printf("\nobstacle colour: %d", test_cameraVision.obstacle_colour);

    sleep(2);

    test_cameraVision = IO_readCamera();
    printf("\n\nobstacle spotted: %d", test_cameraVision.obstacle_spotted);
    printf("\nobstacle x: %d", test_cameraVision.obstacle_x);
    printf("\nobstacle y: %d", test_cameraVision.obstacle_y);
    printf("\nobstacle colour: %d", test_cameraVision.obstacle_colour);

    printf("\n\nDrive forwards slowly.\n");
    IO_writeToDriveMotor(55);
    sleep(1);
    printf("Brake.\n");
    IO_brake();
    sleep(1);
    printf("Drive backwards slowly.\n");
    IO_writeToDriveMotor(-55);
    sleep(1);
    printf("Coast.\n");
    IO_writeToDriveMotor(0);
    sleep(1);
    printf("Drive forwards quickly.\n");
    IO_writeToDriveMotor(150);
    sleep(1);
    printf("Brake.\n");
    IO_brake();
    sleep(1);
    printf("Drive backwards quickly.\n");
    IO_writeToDriveMotor(-150);
    sleep(1);
    printf("Coast.\n");
    IO_writeToDriveMotor(0);
    sleep(1);

    printf("Test steering.\n");
    IO_writeToSteeringMotor(-40, FORWARDS);
    sleep(2);
    IO_writeToSteeringMotor(40, FORWARDS);
    sleep(2);
    IO_writeToSteeringMotor(0, FORWARDS);
    sleep(2);
    IO_writeToSteeringMotor(-40, BACKWARDS);
    sleep(2);
    IO_writeToSteeringMotor(40, BACKWARDS);
    sleep(2);
    IO_writeToSteeringMotor(0, BACKWARDS);
    sleep(2);

    IO_uninitializeDriveMotor();
    IO_writeToSteeringMotor(0, FORWARDS);
    IO_uninitializeSteeringMotor();

    printf("\nthe end.\n");

    return 0;
}