#include "esc.h"
#include <stdio.h>
#include <time.h>

int main() {
    esc_enable();
    esc_set_speed(255);
    sleep(2);
    esc_drive(1);
    sleep(2);
    esc_brake();
    sleep(2);
    esc_drive(0);
    sleep(2);
    esc_coast();
    sleep(2);
    esc_disable();
    esc_servo_steer(30);
    sleep(2);
    esc_servo_steer(-30);
    sleep(2);
    esc_servo_steer(0);
    sleep(2);
    esc_servo_stop();

    return 0;
}