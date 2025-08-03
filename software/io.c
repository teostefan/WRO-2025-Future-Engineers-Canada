#ifndef IO_C
#define IO_C

#include "globals.h"

struct CameraData IO_readCamera() {
    // unfinished
    struct CameraData cameraData;

    return cameraData;
}

float IO_readFrontTOF() {
    // unfinished

    return 0;
}

float IO_readRearTOF() {
    // unfinished

    return 0;
}

float IO_readRightTOF() {
    // unfinished

    return 0;
}

float IO_readLeftTOF() {
    // unfinished

    return 0;
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

float IO_getRawReadingFromGyroscope() {
    // unfinished

    return 0;
}

float IO_readGyroscope(int startDirection, int stretch) {
    // unfinished
    // returns an absolute value

    return 0;
}

int IO_writeToDriveMotor(int speed) {
    // unfinished

    return 0;
}

int IO_writeToSteeringMotor(float angle, int direction) {
    // unfinished

    return 0;
}

#endif // IO_C