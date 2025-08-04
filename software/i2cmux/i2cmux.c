#include "i2cmux.h"

int i2cmux_switch(int channel) {
    static int file_i2c = 0;
    char filename[32];

    sprintf(filename, "/dev/i2c-%d", i2cmux_CHANNEL);
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "I2C Multiplexer Error - Failed to open the i2c bus; try running as sudo.\n");
        return 0;
    }

    if (ioctl(file_i2c, I2C_SLAVE, i2cmux_ADDR) < 0) {
        fprintf(stderr, "I2C Multiplexer Error - Failed to acquire bus access or talk to slave.\n");
        close(file_i2c);
        file_i2c = -1;
        return 0;
    }

    unsigned char buffer[1];

    switch (channel) {
    case 0:
        buffer[0] = 0b00000100;
        break;

    case 1:
        buffer[0] = 0b00000101;
        break;

    case 2:
        buffer[0] = 0b00000110;
        break;

    case 3:
        buffer[0] = 0b00000111;
        break;

    default:
        buffer[0] = 0b00000000;
        break;
    }

    if (write(file_i2c, buffer, 1) != 1) {
    } // suppress warning

    close(file_i2c);
    file_i2c = -1;
    return 1;
}