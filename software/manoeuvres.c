#ifndef MANOEUVRES_C
#define MANOEUVRES_C

#include "globals.h"
#include "io.c"

int pullOut() {
    // Leave parking lot at the beginning of the obstacle challenge
    float orientation;

    // Turn until peeking out of the parking lot
    IO_writeToDriveMotor(CONTROL_SPEED);
    IO_writeToSteeringMotor(PO_SLALOM_STEERING, startDirection);
    do {
        orientation = IO_readGyroscope(startDirection, startDirection, stretch);
    } while (orientation < PULL_OUT_ANGLE);

    // Turn until straight
    IO_writeToSteeringMotor(PO_SLALOM_STEERING, !startDirection);
    do {
        orientation = IO_readGyroscope(startDirection, startDirection, stretch);
    } while (orientation > 0);

    IO_writeToSteeringMotor(STRAIGHT_STEERING, startDirection);

    return 0;
}

int parallelPark() {
    // Parallel park inside of the parking lot at the end of the obstacle challenge
    float frontDistance;
    float rearDistance;
    float orientation;
    float sideDistance;
    float dt;
    float error;
    float correction;
    unsigned long lastTime;
    unsigned long thisTime;
    PID_clearPID(&pid);

    IO_writeToDriveMotor(CONTROL_SPEED);

    // If ended the last lap on the inner side of the track, move to the other side
    if (map[1][0] != startDirection) {
        // Turn until halfway to the desired line
        IO_writeToSteeringMotor(PP_SLALOM_STEERING, !startDirection);
        do {
            orientation = IO_readGyroscope(startDirection, startDirection, stretch);
        } while (orientation < SMALL_SLALOM_ANGLE);

        // Turn until straight
        IO_writeToSteeringMotor(PP_SLALOM_STEERING, startDirection);
        do {
            orientation = IO_readGyroscope(startDirection, startDirection, stretch);
        } while (orientation > 0);

        IO_writeToSteeringMotor(STRAIGHT_STEERING, startDirection);
    }

    lastTime = millis();

    // Using PID controller to stay on the correct trajectory, advance past the parking lot while ignoring obstacles
    do {
        frontDistance = IO_readTOF(VERTICAL, FRONT);
        rearDistance = IO_readTOF(VERTICAL, REAR);
        sideDistance = IO_readTOF(HORIZONTAL, !startDirection);
        if (sideDistance != 0) {
            error = sideDistance - STARTING_SECTION_AVOID_DISTANCE;
        } else {
            error = pid.prev_error;
        }
        thisTime = millis();
        dt = (thisTime - lastTime) / 1000.0f;
        correction = PID_calculatePID(&pid, error, dt);
        IO_writeToSteeringMotor(fabsf(correction), correction > 0);
    } while (frontDistance > PP_FRONT_ADVANCE_DISTANCE || rearDistance < PP_REAR_ADVANCE_DISTANCE);

    IO_writeToDriveMotor(PP_REVERSE_SPEED);

    // Using PID controller to stay on the correct trajectory, reverse until parking lines detected, or parallel park distance reached
    do {
        frontDistance = IO_readTOF(VERTICAL, FRONT);
        rearDistance = IO_readTOF(VERTICAL, REAR);
        if (sideDistance != 0) {
            error = sideDistance - STARTING_SECTION_AVOID_DISTANCE;
        } else {
            error = pid.prev_error;
        }
        thisTime = millis();
        dt = (thisTime - lastTime) / 1000.0f;
        correction = PID_calculatePID(&pid, error, dt);
        IO_writeToSteeringMotor(fabsf(correction), correction > 0);
        sideDistance = IO_readTOF(HORIZONTAL, !startDirection);
    } while ((frontDistance > PP_FRONT_REVERSE_DISTANCE || rearDistance < PP_REAR_REVERSE_DISTANCE) && sideDistance > PP_PARKING_LOT_DISTANCE);

    IO_writeToSteeringMotor(PP_STEERING, startDirection);
    int direction = BACKWARDS;

    // turn until parallel park angle
    do {
        rearDistance = IO_readTOF(VERTICAL, REAR);
        if (rearDistance < PP_WIGGLE_REVERSE_DISTANCE) {
            direction = FORWARDS;
            break;
        }
        orientation = IO_readGyroscope(startDirection, startDirection, stretch);
        orientation = IO_readGyroscope(startDirection, startDirection, stretch);
    } while (orientation < PP_ANGLE);

    // turn until parallel, if wall too close, stop, change direction and turn wheel other way
    do {
        IO_writeToSteeringMotor(PP_STEERING, startDirection ^ direction);
        IO_writeToDriveMotor(PP_REVERSE_SPEED * (direction * 2) - 1);
        if (rearDistance < PP_WIGGLE_REVERSE_DISTANCE) {
            direction = !direction;
        }
        orientation = IO_readGyroscope(startDirection, startDirection, stretch);
    } while (orientation < PP_ACCEPTABLE_MARGIN_ANGLE && orientation > (0 - PP_ACCEPTABLE_MARGIN_ANGLE));

    IO_writeToDriveMotor(STOP);

    return 0;
}

int turnAround(int direction) {
    // turn a round avoiding obstacles
    float orientation;
    int relativeLapDirection = direction ^ startDirection;
    int startingSide = map[stretch][relativeLapDirection];

    IO_writeToSteeringMotor(U_TURN_ANGLE, !startingSide);
    IO_writeToDriveMotor(U_TURN_ANGLE);

    // Turn until turned around
    do {
        orientation = IO_readGyroscope(startDirection, direction, stretch);
    } while (orientation < 180);

    IO_writeToSteeringMotor(STRAIGHT_STEERING, direction);

    return 0;
}

int driveStretch(int direction) {
    // Complete straight section during the obstacle challenge
    float orientation;
    float frontDistance;
    float rearDistance;
    float sideDistance;
    float slalomAngle;
    float dt;
    float error;
    float correction;
    unsigned long lastTime = millis();
    unsigned long thisTime;
    PID_clearPID(&pid);

    int relativeLapDirection = direction ^ startDirection;
    int firstObstacle = map[stretch][relativeLapDirection];
    int secondObstacle = map[stretch][!relativeLapDirection];

    if (map[stretch][0] == map[stretch][1]) {
        // If both obstacles are the same colour, stick to one side all the way down, using PID controller to stay on the correct trajectory
        IO_writeToSteeringMotor(STRAIGHT_STEERING, firstObstacle);
        IO_writeToDriveMotor(MAX_SPEED);
        do {
            frontDistance = IO_readTOF(VERTICAL, FRONT);
            sideDistance = IO_readTOF(HORIZONTAL, firstObstacle);
            if (sideDistance != 0) {
                error = sideDistance - AVOID_DISTANCE;
            } else {
                error = pid.prev_error;
            }
            thisTime = millis();
            dt = (thisTime - lastTime) / 1000.0f;
            correction = PID_calculatePID(&pid, error, dt);
            IO_writeToSteeringMotor(fabsf(correction), correction > 0);
        } while (frontDistance > TURN_DISTANCE);
    } else {
        // If both obstacles are a different colour, stick to one side before switching to the other
        if (stretch == 1) {
            slalomAngle = SMALL_SLALOM_ANGLE;
        } else {
            slalomAngle = BIG_SLALOM_ANGLE;
        }

        // Using PID controller to stay on the correct trajectory, stick to one side until time to switch
        IO_writeToDriveMotor(CONTROL_SPEED);
        do {
            rearDistance = IO_readTOF(VERTICAL, REAR);
            sideDistance = IO_readTOF(HORIZONTAL, firstObstacle);
            if (sideDistance != 0) {
                error = sideDistance - AVOID_DISTANCE;
            } else {
                error = pid.prev_error;
            }
            thisTime = millis();
            dt = (thisTime - lastTime) / 1000.0f;
            correction = PID_calculatePID(&pid, error, dt);
            IO_writeToSteeringMotor(fabsf(correction), correction > 0);
        } while (rearDistance < SLALOM_DISTANCE);

        // Turn until necessary angle for avoiding the obstacle
        IO_writeToSteeringMotor(SLALOM_STEERING, secondObstacle);
        do {
            orientation = IO_readGyroscope(startDirection, direction, stretch);
        } while (orientation < slalomAngle);

        // Turn until straight
        IO_writeToSteeringMotor(SLALOM_STEERING, firstObstacle);
        do {
            orientation = IO_readGyroscope(startDirection, direction, stretch);
        } while (orientation > 0);

        // Using PID controller to stay on the correct trajectory, advance until straight completed
        PID_clearPID(&pid);
        lastTime = millis();
        IO_writeToSteeringMotor(STRAIGHT_STEERING, secondObstacle);
        IO_writeToDriveMotor(MAX_SPEED);
        do {
            frontDistance = IO_readTOF(VERTICAL, FRONT);
            sideDistance = IO_readTOF(HORIZONTAL, secondObstacle);
            if (sideDistance != 0) {
                error = sideDistance - AVOID_DISTANCE;
            } else {
                error = pid.prev_error;
            }
            thisTime = millis();
            dt = (thisTime - lastTime) / 1000.0f;
            correction = PID_calculatePID(&pid, error, dt);
            IO_writeToSteeringMotor(fabsf(correction), correction > 0);
        } while (frontDistance > TURN_DISTANCE);
    }

    return 0;
}

int driveTurn(int direction) {
    // Complete a turning section during the obstacle challenge
    float orientation;
    int relativeLapDirection = direction ^ startDirection;
    int leavingSide = map[stretch][!relativeLapDirection] ^ direction;
    int enteringSide = map[(stretch - (relativeLapDirection * 2 - 1)) % 4][relativeLapDirection] ^ direction;
    float stopAngle;
    float angle2;

    // Set parameters for turn based on the side entered on and the side left from
    // Also take into account whether you are leaving or entering the starting section
    if (leavingSide == WIDE && enteringSide == WIDE) {
        if (((stretch - (relativeLapDirection * 2 - 1)) % 4) == 1) {
            IO_writeToSteeringMotor(WW_SMALL_TURN_ANGLE_1, direction);
            IO_writeToDriveMotor(WW_TURN_SPEED);
            stopAngle = WW_SMALL_STOP_ANGLE;
            angle2 = WW_SMALL_TURN_ANGLE_2;
        } else if (stretch == 1) {
            IO_writeToSteeringMotor(WW_MID_TURN_ANGLE_1, direction);
            IO_writeToDriveMotor(WW_TURN_SPEED);
            stopAngle = WW_MID_STOP_ANGLE;
            angle2 = WW_MID_TURN_ANGLE_2;
        } else {
            IO_writeToSteeringMotor(WW_BIG_TURN_ANGLE_1, direction);
            IO_writeToDriveMotor(WW_TURN_SPEED);
            stopAngle = WW_BIG_STOP_ANGLE;
            angle2 = WW_BIG_TURN_ANGLE_2;
        }
    } else if (leavingSide == WIDE && enteringSide == TIGHT) {
        if (stretch == 1) {
            IO_writeToSteeringMotor(WT_SMALL_TURN_ANGLE_1, direction);
            IO_writeToDriveMotor(WT_TURN_SPEED);
            stopAngle = WT_SMALL_STOP_ANGLE;
            angle2 = WT_SMALL_TURN_ANGLE_2;
        } else {
            IO_writeToSteeringMotor(WT_BIG_TURN_ANGLE_1, direction);
            IO_writeToDriveMotor(WT_TURN_SPEED);
            stopAngle = WT_BIG_STOP_ANGLE;
            angle2 = WT_BIG_TURN_ANGLE_2;
        }
    } else if (leavingSide == TIGHT && enteringSide == WIDE) {
        if (((stretch - (relativeLapDirection * 2 - 1)) % 4) == 1) {
            IO_writeToSteeringMotor(TW_BIG_TURN_ANGLE_1, direction);
            IO_writeToDriveMotor(TW_TURN_SPEED);
            stopAngle = TW_BIG_STOP_ANGLE;
            angle2 = TW_BIG_TURN_ANGLE_2;
        } else {
            IO_writeToSteeringMotor(TW_SMALL_TURN_ANGLE_1, direction);
            IO_writeToDriveMotor(TW_TURN_SPEED);
            stopAngle = TW_SMALL_STOP_ANGLE;
            angle2 = TW_SMALL_TURN_ANGLE_2;
        }
    } else if (leavingSide == TIGHT && enteringSide == TIGHT) {
        IO_writeToSteeringMotor(TT_TURN_ANGLE_1, direction);
        IO_writeToDriveMotor(TT_TURN_SPEED);
        stopAngle = TT_STOP_ANGLE;
        angle2 = TT_TURN_ANGLE_2;
    }

    // Turn until first angle is reached, either directly through the turn or turning wide before coming back in
    do {
        orientation = IO_readGyroscope(startDirection, direction, stretch);
    } while (orientation < stopAngle);

    IO_writeToSteeringMotor(angle2, direction);

    // If turn not yet completed, complete turn
    while (orientation < 90) {
        orientation = IO_readGyroscope(startDirection, direction, stretch);
    }

    IO_writeToSteeringMotor(STRAIGHT_STEERING, direction);

    return 0;
}

int driveOpenStretch(int direction) {
    // Complete straight section during the open challenge, going forwards on laps 1 and 3, and backwards on lap 2
    float frontDistance;
    float sideDistance;
    float dt;
    float error;
    float correction;
    unsigned long lastTime = millis();
    unsigned long thisTime;
    PID_clearPID(&pid);

    // Using PID controller to stay on the correct trajectory, advance until lap complete
    IO_writeToDriveMotor(MAX_SPEED * (((direction == startDirection) * 2) - 1));
    IO_writeToSteeringMotor(STRAIGHT_STEERING, direction);
    do {
        frontDistance = IO_readTOF(VERTICAL, FRONT);
        sideDistance = IO_readTOF(HORIZONTAL, startDirection == RIGHT);
        if (sideDistance != 0) {
            error = sideDistance - OPEN_DISTANCE;
        } else {
            error = pid.prev_error;
        }
        thisTime = millis();
        dt = (thisTime - lastTime) / 1000.0f;
        correction = PID_calculatePID(&pid, error, dt);
        IO_writeToSteeringMotor(fabsf(correction), (correction >= 0) == (direction == startDirection));
    } while (frontDistance > TURN_DISTANCE);

    return 0;
}

int driveOpenTurn(int direction) {
    // Complete turning section during the open challenge
    float orientation;

    IO_writeToSteeringMotor(OPEN_TURN_ANGLE, direction);
    IO_writeToDriveMotor(OPEN_TURN_SPEED * (((direction == startDirection) * 2) - 1));

    // Turn until turn completed
    do {
        orientation = IO_readGyroscope(startDirection, direction, stretch);
    } while (orientation < 90);

    IO_writeToSteeringMotor(STRAIGHT_STEERING, direction);

    return 0;
}

int driveFirstLapStretch(int direction) {
    // unfinished

    return 0;
}

int driveFirstLapTurn(int direction) {
    // unfinished

    return 0;
}

#endif // MANOEUVRES_C