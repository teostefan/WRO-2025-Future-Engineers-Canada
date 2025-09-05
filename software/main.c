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

    sleep(2);

    BNO_init_i2cbus();
    usleep(50000); // 50ms

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

    /*

    map[0] = look();

    if (map[0] == RED_RED) {
        esc_drive(1, 125);

        turn(45, 90);

        go(125, 90, -1, -1, FRONT_MUX, 200);

        turn(-45, 350);

        go(125, 0, -1, -1, FRONT_MUX, 300);
    } else if (map[0] == GREEN_GREEN) {
        esc_drive(1, 125);

        turn(-45, 270);

        go(125, 270, -1, -1, FRONT_MUX, 200);

        turn(45, 10);

        go(125, 0, -1, -1, FRONT_MUX, 300);
    } else if (map[0] == RED_GREEN) {
        esc_drive(1, 125);

        turn(45, 90);

        go(125, 90, -1, -1, FRONT_MUX, 200);

        turn(-45, 350);

        sleep(2);

        turn(-45, 270);

        go(125, 270, -1, -1, FRONT_MUX, 200);

        turn(45, 360);

        go(125, 0, -1, -1, FRONT_MUX, 300);
    } else if (map[0] == GREEN_RED) {
        esc_drive(1, 125);

        turn(-45, 270);

        go(125, 270, -1, -1, FRONT_MUX, 200);

        turn(45, 10);

        sleep(2);

        turn(45, 90);

        go(125, 90, -1, -1, FRONT_MUX, 200);

        turn(-45, 0);

        go(125, 0, -1, -1, FRONT_MUX, 800);
    }

    if (direction == CLOCKWISE && (map[0] == RED_RED || map[0] == GREEN_RED)) {
        turn(-45, 270);

        go(125, 270, -1, -1, FRONT_MUX, 300);

        turn(45, 90);
    } else if (direction == COUNTERCLOCKWISE && (map[0] == GREEN_GREEN || map[0] == RED_GREEN)) {
        turn(45, 90);

        go(125, 270, -1, -1, FRONT_MUX, 300);

        turn(-45, 270);
    } else if (direction == CLOCKWISE) {
        go(125, 0, -1, -1, FRONT_MUX, 500);

        turn(45, 90);
    } else if (direction == COUNTERCLOCKWISE) {
        go(125, 0, -1, -1, FRONT_MUX, 500);

        turn(-45, 270);
    }

    esc_brake();

    */

    /* parallel park on the left side

    esc_drive(0, 125);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(LEFT_MUX);
        usleep(50000); // 50ms
        leftDistance = tofReadDistance();

        error = (int)(0.5 * (zeroHeading - angles.eul_head));
        if (error > 30) {
            error = 30;
        } else if (error < -30) {
            error = -30;
        }

        if (leftDistance < 450) {
            error += 5;
        } else if (leftDistance > 550 && leftDistance < 4000) {
            error -= 5;
        }

        esc_servo_steer(error);

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 4000 || frontDistance < 600);

    esc_servo_steer(-50);

    do {
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf(" first turn Heading = %f degrees ", trueHeading);
    } while (trueHeading < 75 || trueHeading > 180);

    esc_servo_steer(0);

    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 350);

    esc_servo_steer(50);

    // Turn until turn completed
    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 100);

    esc_brake();

    esc_servo_steer(-50);

    esc_drive(1, 110);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 180 || trueHeading > 360);

    */

    /* parallel park on the right side

    esc_drive(0, 125);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(RIGHT_MUX);
        usleep(50000); // 50ms
        rightDistance = tofReadDistance();

        error = (int)(0.5 * (zeroHeading - angles.eul_head));
        if (error > 30) {
            error = 30;
        } else if (error < -30) {
            error = -30;
        }

        if (rightDistance < 450) {
            error -= 5;
        } else if (rightDistance > 550 && rightDistance < 4000) {
            error += 5;
        }

        esc_servo_steer(error);

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 4000 || frontDistance < 650);

    esc_servo_steer(50);

    do {
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 180 || trueHeading > 285);

    esc_servo_steer(0);

    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 300);

    esc_servo_steer(-50);

    // Turn until turn completed
    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 100);

    esc_brake();

    esc_servo_steer(50);

    esc_drive(1, 125);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while ((trueHeading > 180 && trueHeading < 355) || trueHeading > 360);

    */

    do {
        leftDistance = IO_readTOF(HORIZONTAL, LEFT);
        rightDistance = IO_readTOF(HORIZONTAL, RIGHT);
        startDirection = leftDistance <= SEE_WALL_DISTANCE && rightDistance >= SEE_WALL_DISTANCE;
    } while (leftDistance > SEE_WALL_DISTANCE && rightDistance > SEE_WALL_DISTANCE);
    direction = startDirection;

    if (direction == CLOCKWISE) {
        // pull out from the left side
        turn(50, 45);

        int firstObstacle = look();
    } else if (direction == COUNTERCLOCKWISE) {
        // pull out from the right side
        turn(-50, 315);

        int firstObstacle = look();
    }

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