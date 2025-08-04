#include "i2cmux.h"
#include <stdio.h>

int main() {
    popen("i2cdetect -y 1", "r");

    int i = i2cmux_switch(1, 0); // First number is the i2c bus, second is the channel number.

    if (i != 1) {
        printf("Error switching MUX.");
        return -1; // problem - quit
    }
    printf("MUX device successfully opened.\n");

    popen("i2cdetect -y 1", "r");

    return 0;
}