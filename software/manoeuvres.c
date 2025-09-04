#include "globals.h"

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

/* pull out from the left side

esc_servo_steer(50);

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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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

int turn(int way, float targetHeading) {
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

    esc_servo_steer(way * OPEN_TURN_ANGLE);

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
