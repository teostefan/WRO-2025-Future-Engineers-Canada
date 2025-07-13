#ifndef IO_H
#define IO_H

#define DRIVE_SLEEP 27
#define DRIVE_FAULT 21
#define DRIVE_TRQ 22

#define FRONT 1
#define REAR 0

#define RIGHT 1
#define LEFT 0

#define VERTICAL 1
#define HORIZONTAL 0

#define CLOCKWISE 1
#define COUNTERCLOCKWISE 0

#define FORWARDS 1
#define BACKWARDS 0

#define WIDE 1
#define TIGHT 0

#define TOTAL_LAP_NUMBER 3
#define LAP_TURN_NUMBER 4

#define RED 1
#define GREEN 0

struct CameraData {
    int obstacle_spotted;
    int obstacle_x_1;
    int obstacle_x_2;
    int obstacle_y;
    int obstacle_colour;
};

struct CameraData IO_readCamera();

float IO_readTOF(int axis, int looking_direction);

float IO_readFrontTOF();

float IO_readRearTOF();

float IO_readRightTOF();

float IO_readLeftTOF();

float IO_readGyroscope(int startDirection, int stretch);

float IO_getRawReadingFromGyroscope();

int IO_writeToDriveMotor(int speed);

int IO_writeToSteeringMotor(float angle, int direction);

#endif // IO_H