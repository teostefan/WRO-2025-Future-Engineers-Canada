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

    if (BNO_get_eul(&angles) == -1) {
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

do {
    i2cmux_switch(FRONT_MUX);
    usleep(50000); // 50ms
    frontDistance = tofReadDistance();
} while (frontDistance > 300 || frontDistance < 50);

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

    if (BNO_get_eul(&angles) == -1) {
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

    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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
    if (BNO_get_eul(&angles) == -1) {
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

    if (BNO_get_eul(&angles) == -1) {
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

    if (BNO_get_eul(&angles) == -1) {
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

int go(int speed, float straightAngle, int hugMux, int hugTarget, int stopMux, int stopTarget) {
    esc_drive((speed > 0), speed);

    neutralHeading = zeroHeading + straightAngle;
    if (neutralHeading < 360) {
        neutralHeading += 360;
    }
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
            error = (int)(0.5 * (neutralHeading - (angles.eul_head + 360)));
        } else {
            error = (int)(0.5 * ((angles.eul_head + 360) - neutralHeading));
        }

        if (error > 180 || error < 0) {
            error = 0;
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

        usleep(50000); // 50ms
    } while (!(stopDistance > (stopTarget - STOP_DISTANCE_RANGE) && stopDistance < (stopTarget + STOP_DISTANCE_RANGE)));

    return 0;
}

int turn(float angleToTurn) {
    targetHeading = zeroHeading + angleToTurn;
    if (targetHeading > 360) {
        targetHeading -= 360;
    }

    esc_servo_steer(((angleToTurn > 0) * 2 - 1) * OPEN_TURN_ANGLE);

    do {
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        usleep(50000); // 50ms

        trueHeading = angles.eul_head - neutralHeading;
        if (trueHeading < 0) {
            trueHeading += 360;
        }
        printf("Heading = %f degrees ", trueHeading);
    } while (!(trueHeading < (targetHeading + TURN_RANGE) && trueHeading > (targetHeading - TURN_RANGE)));

    esc_servo_steer(0);

    return 0;
}

int turnAround(int startAngle) {
    turn(fmod(startAngle + 90, 360));

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        frontDistance = tofReadDistance();
    } while (frontDistance > 400 || frontDistance < 50);

    turn(fmod(startAngle + 180, 360));

    return 0;
}
