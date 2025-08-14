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

    esc_enable();
    esc_servo_init();
    esc_servo_steer(0);

    // driveOpenChallenge();
    // driveObstacleChallenge();

    esc_disable();
    esc_servo_steer(0);
    esc_servo_uninit();

    return 0;
}