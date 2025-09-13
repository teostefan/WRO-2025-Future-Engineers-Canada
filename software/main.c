#ifndef MAIN_C
#define MAIN_C

#include "globals.h"
#include "manoeuvres.c"

int main() {
    // calibrate sensors
    popen("i2cdetect -y 1", "r");
    for (int i = 0; i < 4; i++) {
        i2cmux_switch(i);
        usleep(50000); // 50ms
        tofInit(1, 0x29, 1);
        usleep(50000); // 50ms
    }

    BNO_init_i2cbus();
    usleep(150000); // 50ms

    do {
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        printf("Zero Heading = %f degrees ", angles.eul_head);
        zeroHeading = angles.eul_head;
        usleep(50000); // 50ms
    } while (zeroHeading < 1 || zeroHeading > 360);

    esc_enable();
    esc_servo_init();
    esc_servo_steer(0);
    esc_coast();

    go(-150, ((0 + direction * ((stretch * 90))) % 360), LEFT_MUX, 165, REAR_MUX, 500, RIGHT_MUX, 0, 0);

    turn(50, 270);

    turn(-50, 30);

    usleep(150000);

    turn(-50, 90);

    go(-150, ((90 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, REAR_MUX, 300, RIGHT_MUX, 0, 0);

    esc_servo_steer(50);

    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 90);

    esc_drive(1, 100);

    turn(-50, 335);

    esc_brake();

    /* CC

    go(-150, ((0 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, REAR_MUX, 450, LEFT_MUX, 0, 0);

    turn(-50, 90);

    turn(50, 330);

    usleep(300000);

    turn(50, 275);

    go(-150, ((270 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, REAR_MUX, 300, LEFT_MUX, 0, 0);

    esc_servo_steer(-50);

    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 100);

    esc_drive(1, 100);

    turn(50, 25);

    esc_brake();

    */

    /*

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        int frontDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        int rearDistance = tofReadDistance();
        usleep(50000); // 50ms

        if (frontDistance <= SEE_WALL_DISTANCE && rearDistance <= SEE_WALL_DISTANCE) {
            challenge = OBSTACLE;
        } else if (frontDistance >= SEE_WALL_DISTANCE && rearDistance >= SEE_WALL_DISTANCE) {
            challenge = OPEN;
        }
    } while ((frontDistance > SEE_WALL_DISTANCE && rearDistance < SEE_WALL_DISTANCE) || (frontDistance <= SEE_WALL_DISTANCE && rearDistance >= SEE_WALL_DISTANCE));

    if (challenge == OPEN) {
        open_challenge(); // see manoeuvres.c for more details
    } else if (challenge == OBSTACLE) {
        obstacle_challenge(); // see manoeuvres.c for more details
    }

    */

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
