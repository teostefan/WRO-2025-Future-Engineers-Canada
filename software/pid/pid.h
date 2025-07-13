#ifndef PID_H
#define PID_H

struct PID_Controller {
    float Kp, Ki, Kd;
    float integral_sum;
    float prev_error;
};

float PID_calculatePID(struct PID_Controller *pid, float error, float dt);

int PID_clearPID(struct PID_Controller *pid);

#endif // PID_H