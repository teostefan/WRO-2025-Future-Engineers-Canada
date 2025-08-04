#include "esc.h"

int esc_set_speed(unsigned char speed) {
    static int file_i2c = 0;
    char filename[32];

    sprintf(filename, "/dev/i2c-%d", esc_CHANNEL);
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "ESC Error - Failed to open the i2c bus for setting motor speed; try running as sudo.\n");
        return 0;
    }

    if (ioctl(file_i2c, I2C_SLAVE, esc_ADDR) < 0) {
        fprintf(stderr, "ESC Error - Failed to acquire bus access or talk to slave for setting motor speed.\n");
        close(file_i2c);
        file_i2c = -1;
        return 0;
    }

    unsigned char buffer[2] = {0x80, speed}; // 0x80 is the command to set the speed.

    if (write(file_i2c, buffer, 2) != 2) {
    } // suppress warning

    return 1;
}

int esc_drive(int direction) {
    static int file_i2c = 0;
    char filename[32];

    sprintf(filename, "/dev/i2c-%d", esc_CHANNEL);
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "ESC Error - Failed to open the i2c bus for starting the motor; try running as sudo.\n");
        return 0;
    }

    if (ioctl(file_i2c, I2C_SLAVE, esc_ADDR) < 0) {
        fprintf(stderr, "ESC Error - Failed to acquire bus access or talk to slave for starting the motor.\n");
        close(file_i2c);
        file_i2c = -1;
        return 0;
    }

    unsigned char buffer[1];

    if (direction) { // Go Forwards
        buffer[0] = 0x82;
    } else { // Go Backwards
        buffer[0] = 0x83;
    }

    if (write(file_i2c, buffer, 1) != 1) {
    } // suppress warning

    return 1;
}

int esc_brake() {
    static int file_i2c = 0;
    char filename[32];

    sprintf(filename, "/dev/i2c-%d", esc_CHANNEL);
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "ESC Error - Failed to open the i2c bus for activating engine brake; try running as sudo.\n");
        return 0;
    }

    if (ioctl(file_i2c, I2C_SLAVE, esc_ADDR) < 0) {
        fprintf(stderr, "ESC Error - Failed to acquire bus access or talk to slave for activating engine brake.\n");
        close(file_i2c);
        file_i2c = -1;
        return 0;
    }

    unsigned char buffer[1] = {0x81}; // 0x81 is the command to enable the engine brake.

    if (write(file_i2c, buffer, 1) != 1) {
    } // suppress warning

    return 1;
}

int esc_coast() {
    static int file_i2c = 0;
    char filename[32];

    sprintf(filename, "/dev/i2c-%d", esc_CHANNEL);
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "ESC Error - Failed to open the i2c bus for stopping the motor via slow decay; try running as sudo.\n");
        return 0;
    }

    if (ioctl(file_i2c, I2C_SLAVE, esc_ADDR) < 0) {
        fprintf(stderr, "ESC Error - Failed to acquire bus access or talk to slave for stopping the motor via slow decay.\n");
        close(file_i2c);
        file_i2c = -1;
        return 0;
    }

    unsigned char buffer[1] = {0x84}; // 0x81 is the command to coast the engine.

    if (write(file_i2c, buffer, 1) != 1) {
    } // suppress warning

    return 1;
}

int esc_tach_reset() {
    static int file_i2c = 0;
    char filename[32];

    sprintf(filename, "/dev/i2c-%d", esc_CHANNEL);
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "ESC Error - Failed to open the i2c bus for resetting the tachometer; try running as sudo.\n");
        return 0;
    }

    if (ioctl(file_i2c, I2C_SLAVE, esc_ADDR) < 0) {
        fprintf(stderr, "ESC Error - Failed to acquire bus access or talk to slave for resetting the tachometer.\n");
        close(file_i2c);
        file_i2c = -1;
        return 0;
    }

    unsigned char buffer[1] = {0x85}; // 0x85 is the command to reset the tachometer.

    if (write(file_i2c, buffer, 1) != 1) {
    } // suppress warning

    return 1;
}

int esc_tach_read(unsigned long *counter) {
    static int file_i2c = 0;
    char filename[32];

    sprintf(filename, "/dev/i2c-%d", esc_CHANNEL);
    if ((file_i2c = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "ESC Error - Failed to open the i2c bus for reading the tachometer; try running as sudo.\n");
        return 0;
    }

    if (ioctl(file_i2c, I2C_SLAVE, esc_ADDR) < 0) {
        fprintf(stderr, "ESC Error - Failed to acquire bus access or talk to slave for reading the tachometer.\n");
        close(file_i2c);
        file_i2c = -1;
        return 0;
    }

    unsigned char buffer[4];

    if (read(file_i2c, &buffer, 4) != 4) {
        fprintf(stderr, "ESC Error - Failed to get correct number of bytes from the tachometer.\n");
        close(file_i2c);
        file_i2c = -1;
        return 0;
    }

    *counter |= (unsigned long)buffer[0]; // Least significant byte
    *counter |= (unsigned long)buffer[1] << 8;
    *counter |= (unsigned long)buffer[2] << 16;
    *counter |= (unsigned long)buffer[3] << 24; // Most significant byte

    close(file_i2c);
    file_i2c = -1;
    return 1;
}