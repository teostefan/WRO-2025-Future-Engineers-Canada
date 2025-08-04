#include "esc.h"

int esc_enable() {
    char command[128];
    sprintf(command, "echo \"%d\" > /sys/class/gpio/export && echo \"out\" > /sys/class/gpio/gpio%d/direction && echo \"1\" > /sys/class/gpio/gpio%d/value", esc_GPIO_PIN, esc_GPIO_PIN, esc_GPIO_PIN);
    if (!popen(command, "w")) {
        fprintf(stderr, "ESC Error - Failed to enable the ESC; try running as sudo.\n");
        return 0;
    }
    return 1;
}

int esc_disable() {
    char command[128];
    sprintf(command, "echo \"%d\" > /sys/class/gpio/unexport && echo \"0\" > /sys/class/gpio/gpio%d/value", esc_GPIO_PIN, esc_GPIO_PIN);
    if (!popen(command, "w")) {
        fprintf(stderr, "ESC Error - Failed to disable the ESC; try running as sudo.\n");
        return 0;
    }
    return 1;
}

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

int esc_servo_steer(int angle) {
    // Steer the connected to PWM pin 13 servo to the specified angle.
    angle += esc_STEERING_SERVO_OFFSET;

    if (angle < esc_STEERING_SERVO_MIN) angle = esc_STEERING_SERVO_MIN;
    if (angle > esc_STEERING_SERVO_MAX) angle = esc_STEERING_SERVO_MAX;

    // dutycycle in nanoseconds
    int duty_cycle = (int)(1500000 + (angle * (500000 / 45))); // 1.5ms is center, 0.5ms per 45 degrees
    int period = 20000000;                                     // 20ms period

    char command[256];
    sprintf(command, "echo \"%d\" > /sys/class/pwm/pwmchip0/export && echo \"%d\" > /sys/class/pwm/pwmchip0/pwm%d/period && echo \"%d\" > /sys/class/pwm/pwmchip0/pwm%d/duty_cycle && echo \"1\" > /sys/class/pwm/pwmchip0/pwm%d/enable", esc_STEERING_SERVO_PIN, period, esc_STEERING_SERVO_PIN, duty_cycle, esc_STEERING_SERVO_PIN, esc_STEERING_SERVO_PIN);

    if (!popen(command, "w")) {
        fprintf(stderr, "ESC Error - Failed to set steering servo angle; try running as sudo.\n");
        return 0;
    }

    return 1;
}

int esc_servo_stop() {
    // Stop the steering servo.
    char command[128];
    sprintf(command, "echo \"0\" > /sys/class/pwm/pwmchip0/pwm%d/enable && echo \"%d\" > /sys/class/pwm/pwmchip0/pwm%d/unexport", esc_STEERING_SERVO_PIN, esc_STEERING_SERVO_PIN, esc_STEERING_SERVO_PIN);

    if (!popen(command, "w")) {
        fprintf(stderr, "ESC Error - Failed to stop steering servo; try running as sudo.\n");
        return 0;
    }

    return 1;
}