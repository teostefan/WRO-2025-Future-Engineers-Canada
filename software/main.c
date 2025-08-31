#ifndef MAIN_C
#define MAIN_C

#include "challenges.c"
#include "globals.h"

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

    // globals
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 2; j++) {
            map[i][j] = -1;
        }
    }
    startDirection = CLOCKWISE;
    float frontDistance;
    float rearDistance;
    float rightDistance;
    float leftDistance;
    int distances[10];
    int error;
    int direction = CLOCKWISE;
    struct bnoeul angles = {0.0, 0.0, 0.0};
    float zeroHeading;
    float trueHeading;
    CV_frame redframe;
    CV_frame greenframe;
    CV_mask redmask;
    CV_mask greenmask;
    CV_bounding_box_list redbboxes = {0};
    CV_bounding_box_list greenbboxes = {0};
    CV_bounding_box emptybox;
    emptybox.x[0] = 0;
    emptybox.x[1] = 0;
    emptybox.y[0] = 0;
    emptybox.y[1] = 0;
    CV_bounding_box *biggestredbox = &emptybox;
    CV_bounding_box *biggestgreenbox = &emptybox;

    do {
        if (BNO_get_eul(&angles, startDirection, startDirection, 1) == -1) {
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
    esc_drive(0, 125);

    // driveOpenChallenge();
    // driveObstacleChallenge();

    do {
        CV_camerapipe camera = CV_getcamera("/dev/video0", "gblur=0.5");
        if (!camera) continue;

        if (!CV_getHSVframe(greenframe, camera)) continue; // Load an HSV frame for detecting green obstacles.
        if (!CV_getHSVframe(redframe, camera)) continue;   // Load an HSV frame for detecting red obstacles.

        // Detect green objects
        CV_chromakey(greenmask, greenframe, H_HM_GREEN, S_HM_GREEN, V_HM_GREEN);
        CV_masktracker(&greenbboxes, greenmask, 50);
        biggestgreenbox = &greenbboxes.boxes[0];
        for (size_t i = 0; i < greenbboxes.count; i++) {
            CV_bounding_box *box = &greenbboxes.boxes[i];
            if (((box->x[1] - box->x[0]) * (box->y[1] - box->y[0])) >= ((biggestgreenbox->x[1] - biggestgreenbox->x[0]) * (biggestgreenbox->y[1] - biggestgreenbox->y[0]))) {
                biggestgreenbox->x[0] = box->x[0];
                biggestgreenbox->x[1] = box->x[1];
                biggestgreenbox->y[0] = box->y[0];
                biggestgreenbox->y[1] = box->y[1];
            }
        }

        // Detect red objects
        CV_chromakey(redmask, redframe, H_HM_RED, S_HM_RED, V_HM_RED);
        CV_masktracker(&redbboxes, redmask, 50);
        biggestredbox = &redbboxes.boxes[0];
        for (size_t i = 0; i < redbboxes.count; i++) {
            CV_bounding_box *box = &redbboxes.boxes[i];
            if (((box->x[1] - box->x[0]) * (box->y[1] - box->y[0])) >= ((biggestredbox->x[1] - biggestredbox->x[0]) * (biggestredbox->y[1] - biggestredbox->y[0]))) {
                biggestredbox->x[0] = box->x[0];
                biggestredbox->x[1] = box->x[1];
                biggestredbox->y[0] = box->y[0];
                biggestredbox->y[1] = box->y[1];
            }
        }

        if (biggestredbox->y[0] == 0 && biggestredbox->y[1] == 0 && biggestgreenbox->y[0] == 0 && biggestgreenbox->y[1] == 0) {
            // no obstacles spotted
        } else if ((((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) < (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) && (biggestredbox->y[1] != 0)) || (biggestgreenbox->y[1] == 0)) {
            // red spotted
        } else if ((((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) > (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) && (biggestgreenbox->y[1] != 0)) || (biggestredbox->y[1] == 0)) {
            // green spotted
        } else {
            // no obstacles spotted
        }

        CV_closecamera(camera);
    } while (biggestredbox->y[0] == 0 && biggestredbox->y[1] == 0 && biggestgreenbox->y[0] == 0 && biggestgreenbox->y[1] == 0);

    /* parallel park on the left side

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
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
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
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
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
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
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
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
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

    /* pull out from the left side

    esc_servo_steer(50);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 70 || trueHeading > 180);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 500 || frontDistance < 100);

    esc_servo_steer(-50);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading > 20);

    esc_drive(0, 100);

    esc_servo_steer(50);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading > 10);

    esc_servo_steer(0);

    */

    /* pull out from the right side

    esc_servo_steer(-50);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 180 || trueHeading > 290);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 500 || frontDistance < 100);

    esc_servo_steer(50);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 330 || trueHeading > 360);

    esc_drive(0, 100);

    esc_servo_steer(-50);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while ((trueHeading > 50 && trueHeading < 350) || trueHeading > 360);

    esc_servo_steer(0);

    */

    /* obstacle tight left

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 1000 || frontDistance < 300);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 70 || trueHeading > 180);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(-OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 180 || trueHeading > 310);

    esc_servo_steer(0);

    */

    /* obstacle tight right

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 1000 || frontDistance < 300);

    esc_servo_steer(-OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 180 || trueHeading > 320);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 70 || trueHeading > 180);

    esc_servo_steer(0);

    */

    /* red-green manoeuvre

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(RIGHT_MUX);
        usleep(50000); // 50ms
        rightDistance = tofReadDistance();
        usleep(50000); // 50ms

        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }

        error = (int)(0.5 * (zeroHeading - angles.eul_head));
        if (error > 30) {
            error = 30;
        } else if (error < -30) {
            error = -30;
        }

        if (rightDistance < 150) {
            error -= 5;
        } else if (rightDistance > 250 && rightDistance < 4000) {
            error += 5;
        }

        esc_servo_steer(error);

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 1500 || frontDistance < 300);

    esc_servo_steer(-OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 180 || trueHeading > 310);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 300 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while ((trueHeading >= 180 && trueHeading <= 320) || (trueHeading < 0 || trueHeading > 360));

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(LEFT_MUX);
        usleep(50000); // 50ms
        leftDistance = tofReadDistance();
        usleep(50000); // 50ms

        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }

        error = (int)(0.5 * (zeroHeading - angles.eul_head));
        if (error > 30) {
            error = 30;
        } else if (error < -30) {
            error = -30;
        }

        if (leftDistance < 150) {
            error += 5;
        } else if (leftDistance > 250 && leftDistance < 4000) {
            error -= 5;
        }

        esc_servo_steer(error);

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 400 || frontDistance < 50);

    */

    /* green-red manoeuvre

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(LEFT_MUX);
        usleep(50000); // 50ms
        leftDistance = tofReadDistance();
        usleep(50000); // 50ms

        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }

        error = (int)(0.5 * (zeroHeading - angles.eul_head));
        if (error > 30) {
            error = 30;
        } else if (error < -30) {
            error = -30;
        }

        if (leftDistance < 150) {
            error += 5;
        } else if (leftDistance > 250 && leftDistance < 4000) {
            error -= 5;
        }

        esc_servo_steer(error);

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 1500 || frontDistance < 300);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 70 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 300 || frontDistance < 50);

    esc_servo_steer(-OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading >= 40 && trueHeading <= 310);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(RIGHT_MUX);
        usleep(50000); // 50ms
        rightDistance = tofReadDistance();
        usleep(50000); // 50ms

        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }

        error = (int)(0.5 * (zeroHeading - angles.eul_head));
        if (error > 30) {
            error = 30;
        } else if (error < -30) {
            error = -30;
        }

        if (rightDistance < 150) {
            error -= 5;
        } else if (rightDistance > 250 && rightDistance < 4000) {
            error += 5;
        }

        esc_servo_steer(error);i2cmux_switch(FRONT_MUX);
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

        if (rightDistance < 150) {
            error -= 5;
        } else if (rightDistance > 250 && rightDistance < 4000) {
            error += 5;
        }

        esc_servo_steer(error);

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 400 || frontDistance < 50);

    */

    /* green-green manoeuvre

    esc_servo_steer(0);

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

        if (leftDistance < 130) {
            error += 5;
        } else if (leftDistance > 230 && leftDistance < 4000) {
            error -= 5;
        }

        esc_servo_steer(error);

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 400 || frontDistance < 50);

    */

    /* red-red manoeuvre

    esc_servo_steer(0);

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

        if (rightDistance < 150) {
            error -= 5;
        } else if (rightDistance > 250 && rightDistance < 4000) {
            error += 5;
        }

        esc_servo_steer(error);

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 400 || frontDistance < 50);

    */

    /* stay straight using gyro

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();

        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }

        esc_servo_steer((int)(0.5 * (zeroHeading - angles.eul_head)));

        trueHeading = angles.eul_head - zeroHeading;

        if (trueHeading < 0) {
            trueHeading += 360;
        }

        usleep(50000); // 50ms
    } while (frontDistance > 400 || frontDistance < 50);

    */

    /* Turn around

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 70 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 150 || trueHeading > 340);

    esc_servo_steer(0);

    */

    /* drive 3 laps with reverse

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 60 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 150 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 240 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while ((trueHeading >= 180 && trueHeading <= 310) || (trueHeading < 0 || trueHeading > 360));

    esc_drive(1, 0);

    esc_servo_steer(0); // end of lap 1
    usleep(50000);      // 50ms

    esc_drive(0, 150);

    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 450 || rearDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = zeroHeading - angles.eul_head;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 60 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 450 || rearDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = zeroHeading - angles.eul_head;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 130 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 475 || rearDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = zeroHeading - angles.eul_head;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 210 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        rearDistance = tofReadDistance();
    } while (rearDistance > 400 || rearDistance < 50); // end of lap 2

    esc_drive(1, 150);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 70 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 150 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (trueHeading < 240 || trueHeading > 340);

    esc_servo_steer(0);

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    esc_servo_steer(OPEN_TURN_ANGLE);

    // Turn until turn completed
    do {
        if (BNO_get_eul(&angles, startDirection, direction, 1) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - zeroHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while ((trueHeading >= 180 && trueHeading <= 320) || (trueHeading < 0 || trueHeading > 360));

    esc_servo_steer(0); // end of lap 3

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