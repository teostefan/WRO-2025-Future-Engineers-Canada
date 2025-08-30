#ifndef IO_C
#define IO_C

#include "globals.h"

struct CV_CameraData IO_readCamera() {
    struct CV_CameraData cameraData;
    CV_frame redframe;
    CV_frame greenframe;
    CV_mask redmask;
    CV_mask greenmask;
    CV_bounding_box_list redbboxes = {0};
    CV_bounding_box_list greenbboxes = {0};

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

    if (biggestredbox->y[0] == 0 && biggestredbox->y[1] == 0 && biggestgreenbox->y[0] == 0 && biggestgreenbox->y[1] == 0) {
        cameraData.obstacle_spotted = 0;
        cameraData.obstacle_x = -1;
        cameraData.obstacle_y = -1;
        cameraData.obstacle_colour = -1;
    } else if ((((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) < (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) && (biggestredbox->y[1] != 0)) || (biggestgreenbox->y[1] == 0)) {
        cameraData.obstacle_spotted = 1;
        cameraData.obstacle_x = (biggestredbox->x[0] + (biggestredbox->x[1] - biggestredbox->x[0]) / 2);
        cameraData.obstacle_y = (biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2);
        cameraData.obstacle_colour = RED;
        if (map[stretch][0] == -1) {
            map[stretch][0] = RED;
        } else if (map[stretch][1] == -1) {
            map[stretch][1] = RED;
        }
    } else if ((((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) > (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) && (biggestgreenbox->y[1] != 0)) || (biggestredbox->y[1] == 0)) {
        cameraData.obstacle_spotted = 1;
        cameraData.obstacle_x = (biggestgreenbox->x[0] + (biggestgreenbox->x[1] - biggestgreenbox->x[0]) / 2);
        cameraData.obstacle_y = (biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2);
        cameraData.obstacle_colour = GREEN;
        if (map[stretch][0] == -1) {
            map[stretch][0] = GREEN;
        } else if (map[stretch][1] == -1) {
            map[stretch][1] = GREEN;
        }
    } else {
        cameraData.obstacle_spotted = 0;
        cameraData.obstacle_x = -1;
        cameraData.obstacle_y = -1;
        cameraData.obstacle_colour = -1;
    }

    CV_closecamera(camera);

    return cameraData;
}

void IO_initializeTOFs() {
    popen("i2cdetect -y 1", "r");
    for (int i = 0; i < 4; i++) {
        i2cmux_switch(i);
        tofInit(1, 0x29, 0);
    }
}

float IO_readTOF(int axis, int looking_direction) {
    int mux;
    int distance = 0;
    int distances[10];
    char *side[] = {
        "Front",
        "Right",
        "Left",
        "Rear"};

    if (axis == VERTICAL) {
        if (looking_direction) {
            mux = 0;
        } else {
            mux = 3;
        }
    } else {
        if (looking_direction) {
            mux = 1;
        } else {
            mux = 2;
        }
    }

    i2cmux_switch(mux);

    for (int i = 0; i < 10; i++) {
        distances[i] = tofReadDistance();
    }
    for (int i = 0; i < 9; i++) {
        if (distances[i] < distances[i + 1]) {
            int temp = distances[i + 1];
            distances[i + 1] = distances[i];
            distances[i] = temp;
        }
    }
    for (int i = 3; i < 7; i++) {
        distance += distances[i];
    }
    distance = distance / 4;

    printf("%s distance = %dmm\n", side[mux], distance);

    return distance;
}

float IO_readGyroscope(int startDirection, int direction, int stretch) {
    struct bnoeul angles = {0.0, 0.0, 0.0};

    if (BNO_get_eul(&angles, startDirection, direction, stretch) == -1) {
        printf("An error occurred.");
    }
    printf("Heading = %lf degrees ", angles.eul_head);
    printf("Roll = %lf degrees ", angles.eul_roll);
    printf("Pitch = %lf degrees\n", angles.eul_pitc);

    return angles.eul_head;
}

int IO_initializeDriveMotor() {
    esc_enable();

    return 0;
}

int IO_uninitializeDriveMotor() {
    esc_disable();

    return 0;
}

int IO_writeToDriveMotor(int speed) {
    if (speed > 0) {
        esc_drive(1, speed);
    } else if (speed < 0) {
        esc_drive(0, speed);
    } else {
        esc_coast();
    }

    return 0;
}

int IO_brake() {
    esc_coast();
    usleep(50000); // 50ms
    esc_brake();

    return 0;
}

int IO_initializeSteeringMotor() {
    esc_servo_init();

    return 0;
}

int IO_uninitializeSteeringMotor() {
    esc_servo_uninit();

    return 0;
}

int IO_writeToSteeringMotor(float angle, int direction) {
    esc_servo_steer((int)angle * (2 * direction - 1));

    return 0;
}

#endif // IO_C