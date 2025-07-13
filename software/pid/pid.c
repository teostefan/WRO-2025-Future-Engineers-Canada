#include "pid.h"

float PID_calculatePID(struct PID_Controller *pid, float error, float dt) {
    if (error == 0) {
        return pid->prev_error;
    } else {
        pid->integral_sum += error * dt;
        float derivative = (error - pid->prev_error) / dt;
        pid->prev_error = error;

        return pid->Kp * error + pid->Ki * pid->integral_sum + pid->Kd * derivative;
    }
}

int PID_clearPID(struct PID_Controller *pid) {
    pid->integral_sum = 0.0f;
    pid->prev_error = 0.0f;

    return 0;
}