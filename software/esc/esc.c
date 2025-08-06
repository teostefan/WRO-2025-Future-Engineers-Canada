#include "esc.h"

int esc_enable() {
    char command[32];
    sprintf(command, "raspi-gpio set %d op dh", esc_GPIO_PIN);
    if (!popen(command, "w")) {
        fprintf(stderr, "ESC Error - Failed to enable the ESC.\n");
        return 0;
    }
    sleep(0.2);
    return 1;
}

int esc_disable() {
    char command[32];
    sprintf(command, "raspi-gpio set %d op dl", esc_GPIO_PIN);
    if (!popen(command, "w")) {
        fprintf(stderr, "ESC Error - Failed to disable the ESC.\n");
        return 0;
    }
    sleep(0.2);
    return 1;
}

int esc_drive(int direction, unsigned char speed) {
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

    speed = (speed >> 4) & 0x0F;

    unsigned char buffer[1];

    if (direction) { // Go Forwards
        buffer[0] = 0x00 | speed;
    } else { // Go Backwards
        buffer[0] = (speed << 4) & 0xF0;
    }

    if (write(file_i2c, buffer, 1) != 1) {
    } // suppress warning

    sleep(0.2);

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

    unsigned char buffer[1] = {0x00}; // 0x00 is the command to enable the engine brake.

    if (write(file_i2c, buffer, 1) != 1) {
    } // suppress warning

    sleep(0.2);

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

    unsigned char buffer[1] = {0xFF}; // 0xFF is the command to coast the engine.

    if (write(file_i2c, buffer, 1) != 1) {
    } // suppress warning

    sleep(0.2);

    return 1;
}

int esc_servo_init() {
    // gpioCfgClock(1, 1, 0);

    if (gpioInitialise() < 0) {
        fprintf(stderr, "ESC - Failed to initialize pigpio for controlling servomotor.\n");
        return 0;
    }

    gpioSetPWMrange(esc_STEERING_SERVO_PIN, 1000);
    gpioSetPWMfrequency(esc_STEERING_SERVO_PIN, 50); // 20ms period.

    return 1;
}

int esc_servo_steer(int angle) {
    // Steer the connected to PWM pin 13 servo to the specified angle in gradians.

    if (angle < esc_STEERING_SERVO_MIN) angle = esc_STEERING_SERVO_MIN;
    if (angle > esc_STEERING_SERVO_MAX) angle = esc_STEERING_SERVO_MAX;

    gpioPWM(esc_STEERING_SERVO_PIN, 25 + (angle / 2 + 50));

    return 1;
}

int esc_servo_uninit() {
    // Stop the steering servo.
    gpioTerminate();

    return 1;
}