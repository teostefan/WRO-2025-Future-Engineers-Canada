#include "globals.h"

int look() {
    CV_frame redframe;
    CV_frame greenframe;

    int counter = 0;
    int obstacles = -1;

    do {
        CV_camerapipe camera = CV_getcamera("/dev/video0", "gblur=0.5");
        if (!camera) continue;

        CV_mask redmask;
        CV_mask greenmask;

        CV_bounding_box_list redbboxes = {0};
        CV_bounding_box_list greenbboxes = {0};

        if (!CV_getHSVframe(greenframe, camera)) continue; // Load an HSV frame for detecting green obstacles.
        if (!CV_getHSVframe(redframe, camera)) continue;   // Load an HSV frame for detecting red obstacles.

        // Detect green objects
        CV_chromakey(greenmask, greenframe, H_HM_GREEN, S_HM_GREEN, V_HM_GREEN);
        CV_masktracker(&greenbboxes, greenmask, 39, 75);

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
        printf("Green box: x=[%d, %d], y=[%d, %d]\n", biggestgreenbox->x[0], biggestgreenbox->x[1], biggestgreenbox->y[0], biggestgreenbox->y[1]);

        // Detect red objects
        CV_chromakey(redmask, redframe, H_HM_RED, S_HM_RED, V_HM_RED);
        CV_masktracker(&redbboxes, redmask, 39, 75);

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
        printf("Red box: x=[%d, %d], y=[%d, %d]\n", biggestredbox->x[0], biggestredbox->x[1], biggestredbox->y[0], biggestredbox->y[1]);

        if (biggestredbox->y[0] == 0 && biggestredbox->y[1] == 0 && biggestgreenbox->y[0] == 0 && biggestgreenbox->y[1] == 0) {
            counter--;
        } else if (biggestredbox->y[0] != 0 && biggestredbox->y[1] != 0 && biggestgreenbox->y[0] == 0 && biggestgreenbox->y[1] == 0) {
            obstacles = RED_RED;
        } else if (biggestredbox->y[0] == 0 && biggestredbox->y[1] == 0 && biggestgreenbox->y[0] != 0 && biggestgreenbox->y[1] != 0) {
            obstacles = GREEN_GREEN;
        } else if ((biggestredbox->y[0] + (biggestredbox->y[1] - biggestredbox->y[0]) / 2) < (biggestgreenbox->y[0] + (biggestgreenbox->y[1] - biggestgreenbox->y[0]) / 2)) {
            obstacles = RED_GREEN;
        } else {
            obstacles = GREEN_RED;
        }

        CV_closecamera(camera);
    } while (counter++ < 1);

    return obstacles;
}

int blind_go(int speed, float neutralHeading, int hugMux, int hugTarget, int stopMux, int stopTarget, int safetyMux, int safetyTarget, int way) {
    int count = 0;
    float targetHeading = neutralHeading + zeroHeading;
    if (targetHeading < 0) {
        targetHeading += 360;
    }
    if (targetHeading > 360) {
        targetHeading -= 360;
    }

    esc_drive((speed > 0), speed);

    do {
        i2cmux_switch(stopMux);
        stopDistance = tofReadDistance();
        i2cmux_switch(safetyMux);
        safetyDistance = tofReadDistance();
        if (stopDistance > stopTarget && safetyDistance > safetyTarget) {
            count++;
        } else {
            count = 0;
        }

        if (hugMux >= 0 && hugTarget >= 0) {
            i2cmux_switch(hugMux);
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
                    error -= 10;
                } else {
                    error += 10;
                }
            } else if (hugDistance > (hugTarget + HUG_DISTANCE_RANGE) && hugDistance < 4000) {
                if (hugMux == RIGHT_MUX) {
                    error += 10;
                } else {
                    error -= 10;
                }
            }
        }

        esc_servo_steer(error);

        printf("stop distance: %d", stopDistance);
    } while ((count < 2 && way) || ((stopDistance > stopTarget || safetyDistance < safetyTarget) && !way));

    return 0;
}

int go(int speed, float neutralHeading, int hugMux, int hugTarget, int stopMux, int stopTarget) {
    float targetHeading = neutralHeading + zeroHeading;
    int count = 0;
    if (targetHeading < 0) {
        targetHeading += 360;
    }
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

        i2cmux_switch(RIGHT_MUX);
        usleep(50000); // 50ms
        if (tofReadDistance() > 4000) {
            count++;
        } else {
            count = 0;
        }

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
    } while (!(stopDistance > minRange && stopDistance < maxRange) && count < 5);

    return 0;
}

int turn(int steeringAngle, float targetHeading) {
    int cycle = 0;
    if (targetHeading < 0) {
        targetHeading += 360;
    }
    if (targetHeading > 360) {
        targetHeading -= 360;
    }
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

int in_in() {
    if (direction == CLOCKWISE) {
        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, RIGHT_MUX, 1200, LEFT_MUX, 0, 1);
    } else if (direction == COUNTERCLOCKWISE) {
        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, LEFT_MUX, 1200, RIGHT_MUX, 0, 1);
    }

    return 0;
}

int in_out() {
    if (direction == CLOCKWISE) {
        turn(-45, ((300 + direction * ((stretch * 90))) % 360));

        go(SLOW_SPEED, ((300 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 250);

        turn(45, ((15 + direction * ((stretch * 90))) % 360));

        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, FRONT_MUX, 500, LEFT_MUX, 50, 0);
    } else if (direction == COUNTERCLOCKWISE) {
        turn(45, ((60 + direction * ((stretch * 90))) % 360));

        go(SLOW_SPEED, ((60 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 250);

        turn(-45, ((345 + direction * ((stretch * 90))) % 360));

        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, FRONT_MUX, 500, RIGHT_MUX, 50, 0);
    }

    return 0;
}

int out_in() {
    if (direction == CLOCKWISE) {
        turn(45, ((60 + direction * ((stretch * 90))) % 360));

        go(SLOW_SPEED, ((60 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 250);

        turn(-45, ((345 + direction * ((stretch * 90))) % 360));

        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, RIGHT_MUX, 1200, LEFT_MUX, 0, 1);
    } else if (direction == COUNTERCLOCKWISE) {
        turn(-45, ((300 + direction * ((stretch * 90))) % 360));

        go(SLOW_SPEED, ((300 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 250);

        turn(45, ((15 + direction * ((stretch * 90))) % 360));

        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, LEFT_MUX, 1200, RIGHT_MUX, 0, 1);
    }

    return 0;
}

int out_out() {
    if (direction == CLOCKWISE) {
        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, FRONT_MUX, 750, LEFT_MUX, 50, 0);
    } else if (direction == COUNTERCLOCKWISE) {
        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, FRONT_MUX, 750, RIGHT_MUX, 50, 0);
    }

    return 0;
}

int in_in_turn() {
    if (direction == CLOCKWISE) {
        esc_drive(0, MEDIUM_SPEED);

        usleep(1500000);

        esc_drive(1, SLOW_SPEED);

        turn(45, ((90 + direction * ((stretch * 90))) % 360));

        blind_go(MEDIUM_SPEED, ((90 + direction * ((stretch * 90))) % 360), -1, -1, REAR_MUX, 500, RIGHT_MUX, 0, 1);

        sleep(1);
    } else if (direction == COUNTERCLOCKWISE) {
        esc_drive(0, MEDIUM_SPEED);

        usleep(1500000);

        esc_drive(1, SLOW_SPEED);

        turn(-45, ((270 + direction * ((stretch * 90))) % 360));

        esc_drive(1, MEDIUM_SPEED);

        blind_go(MEDIUM_SPEED, ((270 + direction * ((stretch * 90))) % 360), -1, -1, REAR_MUX, 500, RIGHT_MUX, 0, 1);

        sleep(1);
    }

    return 0;
}

int in_out_turn() {
    if (direction == CLOCKWISE) {
        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 500, LEFT_MUX, 0, 0);

        turn(45, ((90 + direction * ((stretch * 90))) % 360));

        blind_go(MEDIUM_SPEED, ((90 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, REAR_MUX, 500, RIGHT_MUX, 0, 1);

        sleep(1);
    } else if (direction == COUNTERCLOCKWISE) {
        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 500, RIGHT_MUX, 0, 0);
        turn(-45, ((270 + direction * ((stretch * 90))) % 360));

        blind_go(MEDIUM_SPEED, ((270 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, REAR_MUX, 500, LEFT_MUX, 0, 1);

        sleep(1);
    }

    return 0;
}

int out_in_turn() {
    if (direction == CLOCKWISE) {
        esc_drive(1, SLOW_SPEED);

        turn(45, ((150 + direction * ((stretch * 90))) % 360));

        go(SLOW_SPEED, ((150 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 175);

        turn(45, ((90 + direction * ((stretch * 90))) % 360));

        blind_go(MEDIUM_SPEED, ((90 + direction * ((stretch * 90))) % 360), -1, -1, REAR_MUX, 500, LEFT_MUX, 0, 1);

        sleep(1);
    } else if (direction == COUNTERCLOCKWISE) {
        esc_drive(1, SLOW_SPEED);

        turn(-45, ((210 + direction * ((stretch * 90))) % 360));

        go(SLOW_SPEED, ((210 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 175);

        turn(45, ((270 + direction * ((stretch * 90))) % 360));

        blind_go(MEDIUM_SPEED, ((270 + direction * ((stretch * 90))) % 360), -1, -1, REAR_MUX, 500, RIGHT_MUX, 0, 1);

        sleep(1);
    }

    return 0;
}

int out_out_turn() {
    if (direction == CLOCKWISE) {
        esc_drive(1, SLOW_SPEED);

        turn(45, ((90 + direction * ((stretch * 90))) % 360));

        blind_go(MEDIUM_SPEED, ((90 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, REAR_MUX, 500, RIGHT_MUX, 0, 1);

        sleep(1);
    } else if (direction == COUNTERCLOCKWISE) {
        esc_drive(1, SLOW_SPEED);

        turn(-45, ((270 + direction * ((stretch * 90))) % 360));

        blind_go(MEDIUM_SPEED, ((270 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, REAR_MUX, 500, LEFT_MUX, 0, 1);

        sleep(1);
    }

    return 0;
}

int in_blind_turn() {
    if (direction == CLOCKWISE) {
        esc_drive(1, SLOW_SPEED);

        turn(-45, ((270 + direction * ((stretch * 90))) % 360));

        blind_go(SLOW_SPEED, ((270 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 400, LEFT_MUX, 0, 0);

        turn(45, ((0 + direction * ((stretch * 90))) % 360));

        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 400, LEFT_MUX, 0, 0);

        turn(45, ((115 + direction * ((stretch * 90))) % 360));

        esc_brake();

        map[(stretch + 1) % 4] = look();

        if (map[(stretch + 1) % 4] == GREEN_GREEN || map[(stretch + 1) % 4] == GREEN_RED) {
            esc_drive(1, SLOW_SPEED);

            turn(-45, ((120 + direction * ((stretch * 90))) % 360));

            blind_go(MEDIUM_SPEED, ((90 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, REAR_MUX, 500, LEFT_MUX, 0, 1);

            sleep(1);
        } else {
            esc_drive(1, SLOW_SPEED);

            out_in_turn();
        }
    } else if (direction == COUNTERCLOCKWISE) {
        esc_drive(1, SLOW_SPEED);

        turn(45, ((90 + direction * ((stretch * 90))) % 360));

        blind_go(SLOW_SPEED, ((90 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 400, RIGHT_MUX, 0, 0);

        turn(-45, ((0 + direction * ((stretch * 90))) % 360));

        blind_go(SLOW_SPEED, ((0 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 400, RIGHT_MUX, 0, 0);

        turn(-45, ((245 + direction * ((stretch * 90))) % 360));

        esc_brake();

        map[(stretch + 1) % 4] = look();

        if (map[(stretch + 1) % 4] == GREEN_GREEN || map[(stretch + 1) % 4] == GREEN_RED) {
            esc_drive(1, SLOW_SPEED);

            out_in_turn();
        } else {
            esc_drive(1, SLOW_SPEED);

            turn(45, ((240 + direction * ((stretch * 90))) % 360));

            blind_go(MEDIUM_SPEED, ((270 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, REAR_MUX, 500, LEFT_MUX, 0, 1);

            sleep(1);
        }
    }

    return 0;
}

int out_blind_turn() {
    if (direction == CLOCKWISE) {
        esc_drive(1, SLOW_SPEED);

        turn(45, ((115 + direction * ((stretch * 90))) % 360));

        esc_brake();

        map[(stretch + 1) % 4] = look();

        if (map[(stretch + 1) % 4] == GREEN_GREEN || map[(stretch + 1) % 4] == GREEN_RED) {

            esc_drive(1, SLOW_SPEED);

            turn(-45, ((100 + direction * ((stretch * 90))) % 360));

            blind_go(MEDIUM_SPEED, ((90 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, REAR_MUX, 500, LEFT_MUX, 0, 1);

            sleep(1);
        } else {
            esc_drive(1, SLOW_SPEED);

            out_in_turn();
        }
    } else if (direction == COUNTERCLOCKWISE) {
        esc_drive(1, SLOW_SPEED);

        turn(-45, ((245 + direction * ((stretch * 90))) % 360));

        esc_brake();

        map[(stretch + 1) % 4] = look();

        if (map[(stretch + 1) % 4] == GREEN_GREEN || map[(stretch + 1) % 4] == GREEN_RED) {
            esc_drive(1, SLOW_SPEED);

            out_in_turn();
        } else {
            esc_drive(1, SLOW_SPEED);

            turn(45, ((260 + direction * ((stretch * 90))) % 360));

            blind_go(MEDIUM_SPEED, ((270 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, REAR_MUX, 500, LEFT_MUX, 0, 1);

            sleep(1);
        }
    }

    return 0;
}

int pull_out() {
    if (direction == CLOCKWISE) {
        esc_drive(1, MEDIUM_SPEED);

        turn(50, ((90 + direction * ((stretch * 90))) % 360));

        esc_brake();

        int firstObstacle = look();

        if (firstObstacle == GREEN_GREEN || firstObstacle == GREEN_RED) {
            esc_drive(1, MEDIUM_SPEED);

            turn(45, ((0 + direction * ((stretch * 90))) % 360));

            usleep(500000);

            go(MEDIUM_SPEED, ((0 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, FRONT_MUX, 200);

            out_blind_turn();
        } else {
            esc_drive(1, MEDIUM_SPEED);

            turn(-45, ((120 + direction * ((stretch * 90))) % 360));

            go(MEDIUM_SPEED, ((90 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 200);

            turn(45, ((0 + direction * ((stretch * 90))) % 360));

            blind_go(MEDIUM_SPEED, ((0 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, RIGHT_MUX, 1200, LEFT_MUX, 0, 1);

            in_blind_turn();
        }
    } else if (direction == COUNTERCLOCKWISE) {
        esc_drive(1, MEDIUM_SPEED);

        turn(-50, ((270 + direction * ((stretch * 90))) % 360));

        esc_brake();

        int firstObstacle = look();

        if (firstObstacle == RED_RED || firstObstacle == RED_GREEN) {
            esc_drive(1, MEDIUM_SPEED);

            turn(45, ((0 + direction * ((stretch * 90))) % 360));

            usleep(500000);

            go(MEDIUM_SPEED, ((0 + direction * ((stretch * 90))) % 360), RIGHT_MUX, 200, FRONT_MUX, 200);

            out_blind_turn();
        } else {
            esc_drive(1, MEDIUM_SPEED);

            turn(-45, ((240 + direction * ((stretch * 90))) % 360));

            go(MEDIUM_SPEED, ((270 + direction * ((stretch * 90))) % 360), -1, -1, FRONT_MUX, 200);

            turn(45, ((0 + direction * ((stretch * 90))) % 360));

            blind_go(MEDIUM_SPEED, ((0 + direction * ((stretch * 90))) % 360), LEFT_MUX, 200, LEFT_MUX, 1200, RIGHT_MUX, 0, 1);

            in_blind_turn();
        }
    }

    return 0;
}

int parallel_park() {
    if (direction == CLOCKWISE) {
        turn(45, 90);

        turn(-45, 335);

        go(MEDIUM_SPEED, 90, -1, -1, LEFT_MUX, SEE_WALL_DISTANCE);

        esc_drive(0, SLOW_SPEED);

        turn(-50, 85);

        turn(50, 330);
    } else if (direction == COUNTERCLOCKWISE) {
        turn(-45, 270);

        turn(45, 25);

        go(MEDIUM_SPEED, 270, -1, -1, RIGHT_MUX, SEE_WALL_DISTANCE);

        esc_drive(0, SLOW_SPEED);

        turn(50, 275);

        turn(-50, 30);
    }

    return 0;
}

int obstacle_challenge() {
    do {
        i2cmux_switch(RIGHT_MUX);
        usleep(50000); // 50ms
        rightDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(LEFT_MUX);
        usleep(50000); // 50ms
        leftDistance = tofReadDistance();
        usleep(50000); // 50ms

        if (leftDistance <= SEE_WALL_DISTANCE && rightDistance >= SEE_WALL_DISTANCE) {
            direction = CLOCKWISE;
        } else if (leftDistance >= SEE_WALL_DISTANCE && rightDistance <= SEE_WALL_DISTANCE) {
            direction = COUNTERCLOCKWISE;
        }
    } while ((leftDistance > SEE_WALL_DISTANCE && rightDistance > SEE_WALL_DISTANCE) || (leftDistance <= SEE_WALL_DISTANCE && rightDistance <= SEE_WALL_DISTANCE));

    stretch = 0;

    pull_out();

    for (int i = 1; i < 4; i++) {
        stretch = i % 4;

        if ((map[stretch] == GREEN_GREEN && direction == CLOCKWISE) || (map[stretch] == RED_RED && direction == COUNTERCLOCKWISE)) {
            out_out();
            out_blind_turn();
        } else if ((map[stretch] == GREEN_RED && direction == CLOCKWISE) || (map[stretch] == RED_GREEN && direction == COUNTERCLOCKWISE)) {
            out_in();
            in_blind_turn();
        } else if ((map[stretch] == RED_GREEN && direction == CLOCKWISE) || (map[stretch] == GREEN_RED && direction == COUNTERCLOCKWISE)) {
            in_out();
            out_blind_turn();
        } else if ((map[stretch] == RED_RED && direction == CLOCKWISE) || (map[stretch] == GREEN_GREEN && direction == COUNTERCLOCKWISE)) {
            in_in();
            in_blind_turn();
        }
    }

    for (int i = 0; i < 8; i++) {
        stretch = i % 4;
        next_stretch = (i + 1) % 4;

        if ((map[stretch] == GREEN_GREEN && direction == CLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN)) || (map[stretch] == RED_RED && direction == COUNTERCLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED))) {
            out_out();
            out_in_turn();
        } else if ((map[stretch] == GREEN_GREEN && direction == CLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED)) || (map[stretch] == RED_RED && direction == COUNTERCLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN))) {
            out_out();
            out_out_turn();
        } else if ((map[stretch] == GREEN_RED && direction == CLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN)) || (map[stretch] == RED_GREEN && direction == COUNTERCLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED))) {
            out_in();
            in_in_turn();
        } else if ((map[stretch] == GREEN_RED && direction == CLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED)) || (map[stretch] == RED_GREEN && direction == COUNTERCLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN))) {
            out_in();
            in_out_turn();
        } else if ((map[stretch] == RED_GREEN && direction == CLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN)) || (map[stretch] == GREEN_RED && direction == COUNTERCLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED))) {
            in_out();
            out_in_turn();
        } else if ((map[stretch] == RED_GREEN && direction == CLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED)) || (map[stretch] == GREEN_RED && direction == COUNTERCLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN))) {
            in_out();
            out_out_turn();
        } else if ((map[stretch] == RED_RED && direction == CLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN)) || (map[stretch] == GREEN_GREEN && direction == COUNTERCLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED))) {
            in_in();
            in_in_turn();
        } else if ((map[stretch] == RED_RED && direction == CLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED)) || (map[stretch] == GREEN_GREEN && direction == COUNTERCLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN))) {
            in_in();
            in_out_turn();
        }
    }

    parallel_park();

    return 0;
}

int open_challenge() {
    esc_drive(1, 110);

    i2cmux_switch(RIGHT_MUX);
    usleep(50000); // 50ms
    int sideDistance = tofReadDistance();
    if (sideDistance > 500) {
        turn(45, 70);
        go(110, 70, -1, -1, FRONT_MUX, 300);
        turn(-45, 0);
    }

    go(110, 0, -1, -1, FRONT_MUX, 300);

    i2cmux_switch(RIGHT_MUX);
    usleep(50000); // 50ms
    sideDistance = tofReadDistance();
    if (sideDistance < 2000) {
        mux = LEFT_MUX;
        not_mux = RIGHT_MUX;
        direction = COUNTERCLOCKWISE;
    } else {
        mux = RIGHT_MUX;
        not_mux = LEFT_MUX;
        direction = CLOCKWISE;
    }

    for (int i = 1; i < 12; i++) {
        i2cmux_switch(mux);
        usleep(50000); // 50ms
        int sideDistance = tofReadDistance();
        if (sideDistance > 500) {
            turn(45, ((60 + direction * ((i * 90))) % 360));
            go(110, ((60 + direction * ((i * 90))) % 360), -1, -1, FRONT_MUX, 300);
            turn(-45, ((0 + direction * ((i * 90))) % 360));
        }
        blind_go(110, ((0 + direction * ((i * 90))) % 360), mux, 200, mux, 1200, not_mux, 150, 1);
    }

    turn(100, 0);

    sleep(2);

    esc_brake();

    return 0;
}
