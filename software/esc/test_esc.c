// NOTICE: Run this program as sudo (eg.: `sudo builds/test_esc`) because
// admin privileges are needed to access the PWM controller for the steering servo.
// Any other program that will import "esc.h" (ie. the robot's main program) will
// have to also be run as sudo.

#include "esc.h"
#include <stdio.h>
#include <time.h>

int main() {
    esc_enable();
    printf("Drive forwards slowly.\n");
    esc_drive(1, 55);
    sleep(1);
    printf("Brake.\n");
    esc_brake();
    sleep(1);
    printf("Drive backwards slowly.\n");
    esc_drive(0, 55);
    sleep(1);
    printf("Coast.\n");
    esc_coast();
    sleep(1);
    printf("Drive forwards quickly.\n");
    esc_drive(1, 150);
    sleep(1);
    printf("Brake.\n");
    esc_brake();
    sleep(1);
    printf("Drive backwards quickly.\n");
    esc_drive(0, 150);
    sleep(1);
    printf("Coast.\n");
    esc_coast();
    sleep(1);
    printf("Stop.\n");
    esc_disable();

    printf("Test steering.\n");
    esc_servo_init();
    esc_servo_steer(-40);
    sleep(2);
    esc_servo_steer(40);
    sleep(2);
    esc_servo_steer(0);
    sleep(2);
    esc_servo_uninit();
    printf("Stop.\n");

    return 0;
}