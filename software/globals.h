#ifndef GLOBALS_H
#define GLOBALS_H

#include <math.h>
#include <time.h>

#include "cv/cv.h"
#include "cv/cv_config.h"
#include "gyro/gyro.h"
#include "io/io.h"
#include "pid/pid.h"

#define NOOP
#define millis() (unsigned long)time(NULL)

#define WIDTH 190.0
#define LENGTH 290.0
#define AVOID_DISTANCE (200.0 - (WIDTH / 2))
#define SEE_WALL_DISTANCE 150.0
#define STARTING_SECTION_AVOID_DISTANCE (300.0 - (WIDTH / 2))
#define CENTRE_DISTANCE (500.0 - (WIDTH / 2))
#define OPEN_DISTANCE (500.0 - (WIDTH / 2))
#define TURN_DISTANCE 500.0
#define SLALOM_DISTANCE 7300.0
#define PP_FRONT_ADVANCE_DISTANCE 300.0
#define PP_REAR_ADVANCE_DISTANCE 1000.0
#define PP_FRONT_REVERSE_DISTANCE 500.0
#define PP_REAR_REVERSE_DISTANCE 800.0
#define PP_SIDE_DISTANCE (300.0 - (WIDTH / 2))
#define PP_PARKING_LOT_DISTANCE (100.0 - (WIDTH / 2))
#define PP_ACCEPTABLE_MARGIN_DISTANCE 10.0
#define PP_WIGGLE_ADVANCE_DISTANCE (LENGTH / 2) - PP_ACCEPTABLE_MARGIN_DISTANCE
#define PP_WIGGLE_REVERSE_DISTANCE PP_ACCEPTABLE_MARGIN_DISTANCE

#define MAX_SPEED 255
#define STOP 0
#define CONTROL_SPEED 255
#define WW_TURN_SPEED 255
#define WT_TURN_SPEED 255
#define TW_TURN_SPEED 255
#define TT_TURN_SPEED 255
#define OPEN_TURN_SPEED 255
#define U_TURN_SPEED 255
#define PP_REVERSE_SPEED 255
#define PP_ADVANCE_SPEED 255

#define MAX_STEERING 45.0
#define STRAIGHT_STEERING 45.0
#define SLALOM_STEERING 45.0
#define PO_SLALOM_STEERING 45.0
#define PP_SLALOM_STEERING 45.0
#define PP_STEERING 45.0
#define BIG_SLALOM_ANGLE 45.0
#define SMALL_SLALOM_ANGLE 45.0
#define PP_ANGLE 45.0
#define PP_ACCEPTABLE_MARGIN_ANGLE 5.0

#define OPEN_TURN_ANGLE 45.0
#define U_TURN_ANGLE 45.0
#define PULL_OUT_ANGLE 45.0

#define WW_BIG_TURN_ANGLE_1 45.0
#define WW_BIG_STOP_ANGLE 45.0
#define WW_BIG_TURN_ANGLE_2 45.0
#define WW_MID_TURN_ANGLE_1 45.0
#define WW_MID_STOP_ANGLE 45.0
#define WW_MID_TURN_ANGLE_2 45.0
#define WW_SMALL_TURN_ANGLE_1 45.0
#define WW_SMALL_STOP_ANGLE 45.0
#define WW_SMALL_TURN_ANGLE_2 45.0

#define WT_BIG_TURN_ANGLE_1 45.0
#define WT_BIG_STOP_ANGLE 45.0
#define WT_BIG_TURN_ANGLE_2 45.0
#define WT_SMALL_TURN_ANGLE_1 45.0
#define WT_SMALL_STOP_ANGLE 45.0
#define WT_SMALL_TURN_ANGLE_2 45.0

#define TW_BIG_TURN_ANGLE_1 45.0
#define TW_BIG_STOP_ANGLE 45.0
#define TW_BIG_TURN_ANGLE_2 45.0
#define TW_SMALL_TURN_ANGLE_1 45.0
#define TW_SMALL_STOP_ANGLE 45.0
#define TW_SMALL_TURN_ANGLE_2 45.0

#define TT_TURN_ANGLE_1 45.0
#define TT_STOP_ANGLE 45.0
#define TT_TURN_ANGLE_2 45.0

#define KP 1
#define KI 0
#define KD 0

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

int startDirection;
int map[4][2];
int stretch;
struct PID_Controller pid = {KP, KI, KD, 0.0f, 0.0f};
struct CV_Camera_Data cv_data = {0, 0, 0, 0, 0};

#endif // GLOBALS_H