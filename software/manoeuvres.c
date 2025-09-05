#include "globals.h"

int look() {
    CV_frame redframe;
    CV_frame greenframe;
    CV_mask redmask;
    CV_mask greenmask;
    CV_bounding_box_list redbboxes = {0};
    CV_bounding_box_list greenbboxes = {0};

    int counter = 0;
    int obstacles;

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
            obstacles = RED_RED;
        } else if (biggestredbox->y[0] == 0 && biggestredbox->y[1] == 0 && biggestgreenbox->y[0] != 0 && biggestgreenbox->y[1] != 0) {
            obstacles = GREEN_GREEN;
        } else if ((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) > (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) {
            obstacles = RED_GREEN;
        } else {
            obstacles = GREEN_RED;
        }

        CV_closecamera(camera);
    } while (counter++ < 1);

    return obstacles;
}

int blind_go(int speed, float neutralHeading, int hugMux, int hugTarget, int stopMux, int stopTarget, int way) {
    float targetHeading = neutralHeading + zeroHeading;
    if (targetHeading > 360) {
        targetHeading -= 360;
    }

    esc_drive((speed > 0), speed);

    do {
        i2cmux_switch(stopMux);
        usleep(50000); // 50ms
        stopDistance = tofReadDistance();

        if (hugMux >= 0 && hugTarget >= 0) {
            usleep(50000); // 50ms
            i2cmux_switch(hugMux);
            usleep(50000); // 50ms
            hugDistance = tofReadDistance();
        }

        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }

        if (speed > 0) {
            error = (int)(0.5 * (targetHeading - angles.eul_head));
        } else {
            error = (int)(0.5 * (angles.eul_head - targetHeading));
        }

        if (error > 90) {
            error -= 180;
        } else if (error < -90) {
            error += 180;
        }
        if (error > 30) {
            error = 30;
        } else if (error < -30) {
            error = -30;
        }

        if (hugMux >= 0 && hugTarget >= 0) {
            if (hugDistance < (hugTarget - HUG_DISTANCE_RANGE)) {
                if (hugMux == RIGHT_MUX) {
                    error -= 5;
                } else {
                    error += 5;
                }
            } else if (hugDistance > (hugTarget + HUG_DISTANCE_RANGE) && rightDistance < 4000) {
                if (hugMux == RIGHT_MUX) {
                    error += 5;
                } else {
                    error -= 5;
                }
            }
        }

        esc_servo_steer(error);

        printf("stop distance: %d", stopDistance);

        usleep(50000); // 50ms
    } while (((stopDistance < stopTarget || stopDistance > 4000) && way) || ((stopDistance > stopTarget || stopDistance < 400) && !way));

    return 0;
}

int go(int speed, float neutralHeading, int hugMux, int hugTarget, int stopMux, int stopTarget) {
    float targetHeading = neutralHeading + zeroHeading;
    if (targetHeading > 360) {
        targetHeading -= 360;
    }
    int maxRange = stopTarget + STOP_DISTANCE_RANGE;
    if (maxRange > 4000) {
        maxRange = 4000;
    }
    int minRange = stopTarget - STOP_DISTANCE_RANGE;
    if (minRange < 50) {
        minRange = 50;
    }

    esc_drive((speed > 0), speed);

    do {
        i2cmux_switch(stopMux);
        usleep(50000); // 50ms
        stopDistance = tofReadDistance();

        if (hugMux >= 0 && hugTarget >= 0) {
            usleep(50000); // 50ms
            i2cmux_switch(hugMux);
            usleep(50000); // 50ms
            hugDistance = tofReadDistance();
        }

        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }

        if (speed > 0) {
            error = (int)(0.5 * (targetHeading - angles.eul_head));
        } else {
            error = (int)(0.5 * (angles.eul_head - targetHeading));
        }

        if (error > 180 || error < -180) {
            error = 0;
        } else if (error > 90) {
            error -= 180;
        } else if (error < -90) {
            error += 180;
        }

        if (error > 30) {
            error = 30;
        } else if (error < -30) {
            error = -30;
        }

        if (hugMux >= 0 && hugTarget >= 0) {
            if (hugDistance < (hugTarget - HUG_DISTANCE_RANGE)) {
                if (hugMux == RIGHT_MUX) {
                    error -= 5;
                } else {
                    error += 5;
                }
            } else if (hugDistance > (hugTarget + HUG_DISTANCE_RANGE) && rightDistance < 4000) {
                if (hugMux == RIGHT_MUX) {
                    error += 5;
                } else {
                    error -= 5;
                }
            }
        }

        esc_servo_steer(error);

        printf("stop distance: %d", stopDistance);

        usleep(50000); // 50ms
    } while (!(stopDistance > minRange && stopDistance < maxRange));

    return 0;
}

int turn(int steeringAngle, float targetHeading) {
    int cycle = 0;
    int maxRange = targetHeading + TURN_RANGE;
    if (maxRange > 360) {
        maxRange -= 360;
        cycle = 1;
    }
    int minRange = targetHeading - TURN_RANGE;
    if (minRange < 0) {
        minRange += 360;
        cycle = 1;
    }

    esc_servo_steer(steeringAngle);

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
    } while ((trueHeading > maxRange && trueHeading < minRange && cycle) || ((trueHeading > maxRange || trueHeading < minRange) && !cycle));

    esc_servo_steer(0);

    return 0;
}