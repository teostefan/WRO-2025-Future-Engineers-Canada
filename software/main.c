#ifndef MAIN_C
#define MAIN_C

#include "globals.h"
#include "manoeuvres.c"

int main() {
    //  calibrate sensors
    popen("i2cdetect -y 1", "r");
    for (int i = 0; i < 4; i++) {
        i2cmux_switch(i);
        usleep(50000); // 50ms
        tofInit(1, 0x29, 1);
        usleep(50000); // 50ms
    }

    BNO_init_i2cbus();
    usleep(50000); // 50ms

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            map[i][j] = -1;
        }
    }

    do {
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        printf("Zero Heading = %f degrees ", angles.eul_head);
        zeroHeading = angles.eul_head;
        usleep(50000); // 50ms
        if (zeroHeading < 0) {
            zeroHeading += 360;
        }
    } while (zeroHeading < 1 || zeroHeading > 360);

    esc_enable();
    esc_servo_init();
    esc_servo_steer(0);
    esc_coast();

    // driveOpenChallenge();
    // driveObstacleChallenge();

    // drive 3 laps with reverse

    stretch = 1;
    direction = CLOCKWISE;
    startDirection = CLOCKWISE;

    go(100, 0, -1, -1, FRONT_MUX, 400);

    turn(90);

    go(100, 90, -1, -1, FRONT_MUX, 400);

    turn(180);

    go(100, 180, -1, -1, FRONT_MUX, 400);

    turn(270);

    go(100, 270, -1, -1, FRONT_MUX, 400);

    turn(360);

    printf("stopping");

    esc_coast();
    sleep(2);
    esc_servo_steer(0);
    sleep(2);
    esc_disable();
    esc_servo_uninit();

    printf("\nthe end.\n");

    return 0;
}

#endif // MAIN_C