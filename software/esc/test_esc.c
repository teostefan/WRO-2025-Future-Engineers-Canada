#include "esc.h"
#include <stdio.h>
#include <time.h>

int main() {
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

    return 0;
}