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

    CV_frame redframe;
    CV_frame greenframe;
    CV_mask redmask;
    CV_mask greenmask;
    CV_bounding_box_list redbboxes = {0};
    CV_bounding_box_list greenbboxes = {0};

    int counter = 0;

    do {
        CV_camerapipe camera = CV_getcamera("/dev/video0", "gblur=0.5");
        if (!camera) continue;

        if (!CV_getHSVframe(greenframe, camera)) continue; // Load an HSV frame for detecting green obstacles.
        if (!CV_getHSVframe(redframe, camera)) continue;   // Load an HSV frame for detecting red obstacles.

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
            counter--;
        } else if (biggestredbox->y[0] != 0 && biggestredbox->y[1] != 0 && biggestgreenbox->y[0] == 0 && biggestgreenbox->y[1] == 0) {
            map[0] = RED_RED;
        } else if (biggestredbox->y[0] == 0 && biggestredbox->y[1] == 0 && biggestgreenbox->y[0] != 0 && biggestgreenbox->y[1] != 0) {
            map[0] = GREEN_GREEN;
        } else if ((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) > (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) {
            map[0] = RED_GREEN;
        } else {
            map[0] = GREEN_RED;
        }

        CV_closecamera(camera);
    } while (counter++ < 2);

    if (map[0] == RED_RED) {
        esc_drive(1, 125);

        turn(1, 90);

        go(125, 90, -1, -1, FRONT_MUX, 200);

        turn(-1, 350)

            go(125, 0, -1, -1, FRONT_MUX, 300);
    } else if (map[0] == GREEN_GREEN) {
        esc_drive(1, 125);

        turn(-1, 270);

        go(125, 270, -1, -1, FRONT_MUX, 200);

        turn(1, 10);

        go(125, 0, -1, -1, FRONT_MUX, 300);
    } else if (map[0] == RED_GREEN) {
        esc_drive(1, 125);

        turn(1, 90);

        go(125, 90, -1, -1, FRONT_MUX, 200);

        turn(-1, 350);

        sleep(2);

        turn(-1, 270);

        go(125, 270, -1, -1, FRONT_MUX, 200);

        turn(1, 360);

        go(125, 0, -1, -1, FRONT_MUX, 300);
    } else if (map[0] == GREEN_RED) {
        esc_drive(1, 125);

        turn(-1, 270);

        go(125, 270, -1, -1, FRONT_MUX, 200);

        turn(1, 10);

        sleep(2);

        turn(1, 90);

        go(125, 90, -1, -1, FRONT_MUX, 200);

        turn(-1, 0);

        go(125, 0, -1, -1, FRONT_MUX, 300);
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
