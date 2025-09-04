#include "gyro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    int i;
    struct bnoeul angles = {0.0, 0.0, 0.0};

    // For Raspberry Pi's, the I2C channel is usually 1
    // For other boards (e.g. OrangePi) it's 0
    BNO_init_i2cbus();
    if (BNO_get_eul(&angles) == -1) {
        printf("An error occurred.");
    }

    for (i = 0; i < 100; i++) { // read values 20 times a second for 1 minute
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        printf("Heading = %lf degrees ", angles.eul_head);
        printf("Roll = %lf degrees ", angles.eul_roll);
        printf("Pitch = %lf degrees\n", angles.eul_pitc);
        usleep(50000); // 50ms
    }

    return 0;
}