#ifndef IO_C
#define IO_C

#include "globals.h"

struct CV_CameraData IO_readCamera() {
    struct CV_CameraData cameraData;

    CV_frame redframe;
    CV_frame greenframe;
    int frame_count = 0;

    CV_mask redmask;
    CV_mask greenmask;
    CV_bounding_box_list redbboxes = {0};
    CV_bounding_box_list greenbboxes = {0};

    while (frame_count++ < 40) { // Limit to 100 frames for demonstration
        CV_camerapipe camera = CV_getcamera("/dev/video0", "gblur=0.5");
        if (!camera) cameraData.obstacle_spotted = 0;

        if (!CV_getHSVframe(greenframe, camera)) cameraData.obstacle_spotted = 0; // Load an HSV frame for detecting green obstacles.
        if (!CV_getHSVframe(redframe, camera)) cameraData.obstacle_spotted = 0;   // Load an HSV frame for detecting red obstacles.

        // Detect green objects
        CV_chromakey(greenmask, greenframe, H_HM_GREEN, S_HM_GREEN, V_HM_GREEN);
        CV_masktracker(&greenbboxes, greenmask, 50);
        CV_bounding_box *biggestgreenbox = &greenbboxes.boxes[0];
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
        CV_bounding_box *biggestredbox = &redbboxes.boxes[0];
        for (size_t i = 0; i < redbboxes.count; i++) {
            CV_bounding_box *box = &redbboxes.boxes[i];
            if (((box->x[1] - box->x[0]) * (box->y[1] - box->y[0])) >= ((biggestredbox->x[1] - biggestredbox->x[0]) * (biggestredbox->y[1] - biggestredbox->y[0]))) {
                biggestredbox->x[0] = box->x[0];
                biggestredbox->x[1] = box->x[1];
                biggestredbox->y[0] = box->y[0];
                biggestredbox->y[1] = box->y[1];
            }
        }

        if ((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) > (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) {
            cameraData.obstacle_spotted = 1;
            cameraData.obstacle_x = (biggestredbox->x[0] + (biggestredbox->x[1] - biggestredbox->x[0]) / 2);
            cameraData.obstacle_y = (biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2);
            cameraData.obstacle_colour = RED;
        } else if ((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) < (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) {
            cameraData.obstacle_spotted = 1;
            cameraData.obstacle_x = (biggestgreenbox->x[0] + (biggestgreenbox->x[1] - biggestgreenbox->x[0]) / 2);
            cameraData.obstacle_y = (biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2);
            cameraData.obstacle_colour = GREEN;
        } else {
            cameraData.obstacle_spotted = 0;
        }

        CV_closecamera(camera);
    }

    return cameraData;
}

float IO_readFrontTOF() {
    popen("i2cdetect -y 1", "r");

    int i = i2cmux_switch(0);

    if (i != 1) {
        printf("Error switching MUX.");
    }
    printf("MUX device successfully opened.\n");

    popen("i2cdetect -y 1", "r");

    int iDistance;
    int model, revision;

    i = tofInit(1, 0x29, 0);
    if (i != 1) {
        printf("kaput");
    }
    printf("VL53L0X device successfully opened.\n");
    i = tofGetModel(&model, &revision);
    printf("Model ID - %d\n", model);
    printf("Revision ID - %d\n", revision);

    iDistance = tofReadDistance();
    // if (iDistance < 4096) // valid range?
    printf("Distance = %dmm\n", iDistance);

    return iDistance;
}

float IO_readRearTOF() {
    popen("i2cdetect -y 1", "r");

    int i = i2cmux_switch(3);

    if (i != 1) {
        printf("Error switching MUX.");
    }
    printf("MUX device successfully opened.\n");

    popen("i2cdetect -y 1", "r");

    int iDistance;
    int model, revision;

    i = tofInit(1, 0x29, 0);
    if (i != 1) {
        printf("kaput");
    }
    printf("VL53L0X device successfully opened.\n");
    i = tofGetModel(&model, &revision);
    printf("Model ID - %d\n", model);
    printf("Revision ID - %d\n", revision);

    iDistance = tofReadDistance();
    // if (iDistance < 4096) // valid range?
    printf("Distance = %dmm\n", iDistance);

    return iDistance;
}

float IO_readRightTOF() {
    popen("i2cdetect -y 1", "r");

    int i = i2cmux_switch(1);

    if (i != 1) {
        printf("Error switching MUX.");
    }
    printf("MUX device successfully opened.\n");

    popen("i2cdetect -y 1", "r");

    int iDistance;
    int model, revision;

    i = tofInit(1, 0x29, 0);
    if (i != 1) {
        printf("kaput");
    }
    printf("VL53L0X device successfully opened.\n");
    i = tofGetModel(&model, &revision);
    printf("Model ID - %d\n", model);
    printf("Revision ID - %d\n", revision);

    iDistance = tofReadDistance();
    // if (iDistance < 4096) // valid range?
    printf("Distance = %dmm\n", iDistance);

    return iDistance;
}

float IO_readLeftTOF() {
    popen("i2cdetect -y 1", "r");

    int i = i2cmux_switch(2);

    if (i != 1) {
        printf("Error switching MUX.");
    }
    printf("MUX device successfully opened.\n");

    popen("i2cdetect -y 1", "r");

    int iDistance;
    int model, revision;

    i = tofInit(1, 0x29, 0);
    if (i != 1) {
        printf("kaput");
    }
    printf("VL53L0X device successfully opened.\n");
    i = tofGetModel(&model, &revision);
    printf("Model ID - %d\n", model);
    printf("Revision ID - %d\n", revision);

    iDistance = tofReadDistance();
    // if (iDistance < 4096) // valid range?
    printf("Distance = %dmm\n", iDistance);

    return iDistance;
}

float IO_readTOF(int axis, int looking_direction) {
    if (axis == VERTICAL) {
        if (looking_direction) {
            return IO_readFrontTOF();
        } else {
            return IO_readRearTOF();
        }
    } else {
        if (looking_direction) {
            return IO_readRightTOF();
        } else {
            return IO_readLeftTOF();
        }
    }
}

float IO_readGyroscope(int startDirection, int direction, int stretch) {
    struct bnoeul angles = {0.0, 0.0, 0.0};

    if (BNO_get_eul(&angles, startDirection, direction, stretch) == -1) {
        printf("An error occurred.");
    }
    printf("Heading = %lf degrees ", angles.eul_head);
    printf("Roll = %lf degrees ", angles.eul_roll);
    printf("Pitch = %lf degrees\n", angles.eul_pitc);
    usleep(50000); // 50ms

    return angles.eul_head;
}

int IO_writeToDriveMotor(int speed) {
    if (speed > 0) {
        esc_drive(1, speed);
    } else if (speed < 0) {
        esc_drive(0, -speed);
    } else {
        esc_coast();
        usleep(50000); // 50ms
        esc_brake();
    }

    return 0;
}

int IO_writeToSteeringMotor(float angle, int direction) {
    esc_servo_steer((int)angle * (2 * direction - 1));

    return 0;
}

#endif // IO_C