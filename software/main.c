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

    esc_drive(1, 110);

    for (int i = 0; i < 12; i++) {
        i2cmux_switch(RIGHT_MUX);
        usleep(50000); // 50ms
        int sideDistance = tofReadDistance();
        if (sideDistance > 500) {
            turn(45, 60 + ((i * 90) % 360));
            go(110, 60 + ((i * 90) % 360), -1, -1, FRONT_MUX, 300);
            turn(-45, 0 + ((i * 90) % 360));
        }
        blind_go(110, 0 + ((i * 90) % 360), RIGHT_MUX, 200, RIGHT_MUX, 1200, LEFT_MUX, 150, 1);

        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        int turnOneDistance = tofReadDistance();
        printf("turn one distance: %d\n", turnOneDistance);
    }

    turn(90, 0);

    sleep(1);

    esc_brake();

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

    /*

    do {
        i2cmux_switch(LEFT_MUX);
        usleep(50000); // 50ms
        leftDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(RIGHT_MUX);
        usleep(50000); // 50ms
        rightDistance = tofReadDistance();
        usleep(50000); // 50ms
        if (leftDistance <= SEE_WALL_DISTANCE && rightDistance >= SEE_WALL_DISTANCE) {
            startDirection = CLOCKWISE;
        } else {
            startDirection = COUNTERCLOCKWISE;
        }
    } while (leftDistance > SEE_WALL_DISTANCE && rightDistance > SEE_WALL_DISTANCE);
    direction = startDirection;

    if (direction == CLOCKWISE) {
        // pull out from the left side
        esc_drive(1, 125);

        turn(50, 90);

        esc_brake();

        firstObstacle = look();

        if (firstObstacle == RED_RED || firstObstacle == RED_GREEN) {
            esc_drive(1, 100);

            turn(50, 120);

            go(100, 90, -1, -1, FRONT_MUX, 100);

            turn(-45, 0);

            blind_go(100, 0, -1, -1, FRONT_MUX, 1000, FRONT_MUX, 0, 0);
        } else {
            esc_drive(1, 100);

            turn(-30, 0);

            blind_go(100, 0, -1, -1, FRONT_MUX, 1000, FRONT_MUX, 0, 0);
        }
    } else if (direction == COUNTERCLOCKWISE) {
        // pull out from the right side
        esc_drive(1, 100);

        turn(-50, 270);

        esc_brake();

        int firstObstacle = look();

        if (firstObstacle == RED_RED || firstObstacle == RED_GREEN) {
            esc_drive(1, 100);

            turn(30, 0);

            blind_go(100, 0, -1, -1, FRONT_MUX, 1000, FRONT_MUX, 0, 0);
        } else {
            esc_drive(1, 100);

            turn(-45, 240);

            go(100, 270, -1, -1, FRONT_MUX, 100);

            turn(45, 0);

            blind_go(100, 0, -1, -1, FRONT_MUX, 1000, FRONT_MUX, 0, 0);
        }
    }

    if (direction == CLOCKWISE && (firstObstacle == RED_RED || firstObstacle == RED_GREEN)) {
        turn(-45, 270);

        go(100, 270, -1, -1, FRONT_MUX, 200);

        turn(45, 120);
    } else if (direction == COUNTERCLOCKWISE && (firstObstacle == GREEN_GREEN || firstObstacle == GREEN_RED)) {
        turn(45, 90);

        go(100, 90, -1, -1, FRONT_MUX, 200);

        turn(-45, 300);
    } else if (direction == CLOCKWISE) {
        go(100, 0, -1, -1, FRONT_MUX, 500);

        turn(45, 120);
    } else if (direction == COUNTERCLOCKWISE) {
        go(100, 0, -1, -1, FRONT_MUX, 500);

        turn(-45, 300);
    }

    esc_brake();

    int i = 1;

    map[i] = look();

    if (map[i] == RED_RED) {
        esc_drive(1, 100);

        turn(45, 90 + (i * direction * 90));

        turn(-30, 350 + (i * direction * 90));

        blind_go(100, 0 + (i * direction * 90), -1, -1, FRONT_MUX, 1000, FRONT_MUX, 0, 0);
    } else if (map[i] == GREEN_GREEN) {
        esc_drive(1, 100);

        turn(-45, 270 + (i * direction * 90));

        turn(30, 10 + (i * direction * 90));

        blind_go(100, 0 + (i * direction * 90), -1, -1, FRONT_MUX, 1000, FRONT_MUX, 0, 0);
    } else if (map[i] == RED_GREEN) {
        esc_drive(1, 100);

        turn(45, 90 + (i * direction * 90));

        turn(-30, 350 + (i * direction * 90));

        sleep(1);

        turn(-45, 225 + (i * direction * 90));

        go(100, 225 + (i * direction * 90), -1, -1, FRONT_MUX, 200);

        turn(45, 0 + (i * direction * 90));

        blind_go(100, 0 + (i * direction * 90), -1, -1, FRONT_MUX, 1000, FRONT_MUX, 0, 0);
    } else if (map[i] == GREEN_RED) {
        esc_drive(1, 100);

        turn(-45, 270 + (i * direction * 90));

        turn(30, 10 + (i * direction * 90));

        sleep(1);

        turn(45, 45 + (i * direction * 90));

        go(100, 45 + (i * direction * 90), -1, -1, FRONT_MUX, 200);

        turn(-45, 0 + (i * direction * 90));

        blind_go(100, 0 + (i * direction * 90), -1, -1, FRONT_MUX, 1000, FRONT_MUX, 0, 0);
    }

    if (direction == CLOCKWISE && (map[i] == RED_RED || map[i] == GREEN_RED)) {
        turn(-45, 270 + (i * direction * 90));

        go(100, 270 + (i * direction * 90), -1, -1, FRONT_MUX, 300);

        turn(45, 90 + (i * direction * 90));
    } else if (direction == COUNTERCLOCKWISE && (map[i] == GREEN_GREEN || map[i] == RED_GREEN)) {
        turn(45, 90 + (i * direction * 90));

        go(100, 90 + (i * direction * 90), -1, -1, FRONT_MUX, 300);

        turn(-45, 270 + (i * direction * 90));
    } else if (direction == CLOCKWISE) {
        go(100, 0 + (i * direction * 90), -1, -1, FRONT_MUX, 500);

        turn(45, 90 + (i * direction * 90));
    } else if (direction == COUNTERCLOCKWISE) {
        go(100, 0 + (i * direction * 90), -1, -1, FRONT_MUX, 500);

        turn(-45, 270 + (i * direction * 90));
    }

    esc_brake();

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
