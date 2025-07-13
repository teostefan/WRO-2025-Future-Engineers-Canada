#include "gyro.h"

/* ------------------------------------------------------------ *
 * global variables                                             *
 * ------------------------------------------------------------ */
int i2cfd; // I2C file descriptor

/* ------------------------------------------------------------ *
 * BNO_init_i2cbus - Enables the I2C bus communication. Raspberry  *
 * Pi 2 uses i2c-1, RPI 1 used i2c-0, NanoPi also uses i2c-0.   *
 * ------------------------------------------------------------ */
void BNO_init_i2cbus(char *i2cbus, char *i2caddr) {

    if ((i2cfd = open(i2cbus, O_RDWR)) < 0) {
        printf("Error failed to open I2C bus [%s].\n", i2cbus);
        exit(-1);
    }
    if (VERBOSE) printf("Debug: I2C bus device: [%s]\n", i2cbus);
    /* --------------------------------------------------------- *
     * Set I2C device (BNO055 I2C address is  0x28 or 0x29)      *
     * --------------------------------------------------------- */
    int addr = (int)strtol(i2caddr, NULL, 16);
    if (VERBOSE) printf("Debug: Sensor address: [0x%02X]\n", addr);

    if (ioctl(i2cfd, I2C_SLAVE, addr) != 0) {
        printf("Error can't find sensor at address [0x%02X].\n", addr);
        exit(-1);
    }
    /* --------------------------------------------------------- *
     * I2C communication test is the only way to confirm success *
     * --------------------------------------------------------- */
    char reg = BNO055_CHIP_ID_ADDR;
    if (write(i2cfd, &reg, 1) != 1) {
        printf("Error: I2C write failure register [0x%02X], sensor addr [0x%02X]?\n", reg, addr);
        exit(-1);
    }
}

/* --------------------------------------------------------------- *
 * bno_reset() resets the sensor. It will come up in CONFIG mode.  *
 * --------------------------------------------------------------- */
int BNO_reset() {
    char data[2];
    data[0] = BNO055_SYS_TRIGGER_ADDR;
    data[1] = 0x20;
    if (write(i2cfd, data, 2) != 2) {
        printf("Error: I2C write failure for register 0x%02X\n", data[0]);
        exit(-1);
    }
    if (VERBOSE) printf("Debug: BNO055 Sensor Reset complete\n");

    /* ------------------------------------------------------------ *
     * After a reset, the sensor needs at leat 650ms to boot up.    *
     * ------------------------------------------------------------ */
    usleep(650 * 1000);
    exit(0);
}

/* ------------------------------------------------------------ *
 *  get_mag() - read magnetometer data into the global struct   *
 *  Convert magnetometer data in microTesla. 1 microTesla = 16  *
 * ------------------------------------------------------------ */
int BNO_get_mag(struct bnomag *bnod_ptr) {
    char reg = BNO055_MAG_DATA_X_LSB_ADDR;
    if (write(i2cfd, &reg, 1) != 1) {
        printf("Error: I2C write failure for register 0x%02X\n", reg);
        return (-1);
    }

    char data[6] = {0};
    if (read(i2cfd, data, 6) != 6) {
        printf("Error: I2C read failure for register data 0x%02X\n", reg);
        return (-1);
    }

    int16_t buf = ((int16_t)data[1] << 8) | data[0];
    if (VERBOSE) printf("Debug: Magnetometer Data X: LSB [0x%02X] MSB [0x%02X] INT16 [%d]\n", data[0], data[1], buf);
    bnod_ptr->mdata_x = (double)buf / 1.6;

    buf = ((int16_t)data[3] << 8) | data[2];
    if (VERBOSE) printf("Debug: Magnetometer Data Y: LSB [0x%02X] MSB [0x%02X] INT16 [%d]\n", data[2], data[3], buf);
    bnod_ptr->mdata_y = (double)buf / 1.6;

    buf = ((int16_t)data[5] << 8) | data[4];
    if (VERBOSE) printf("Debug: Magnetometer Data Z: LSB [0x%02X] MSB [0x%02X] INT16 [%d]\n", data[4], data[5], buf);
    bnod_ptr->mdata_z = (double)buf / 1.6;
    return (0);
}

/* ------------------------------------------------------------ *
 *  get_eul() - read Euler orientation into the global struct   *
 * ------------------------------------------------------------ */
int BNO_get_eul(struct bnoeul *bnod_ptr) {
    char reg = BNO055_EULER_H_LSB_ADDR;
    if (write(i2cfd, &reg, 1) != 1) {
        printf("Error: I2C write failure for register 0x%02X\n", reg);
        return (-1);
    }

    if (VERBOSE) printf("Debug: I2C read 6 bytes starting at register 0x%02X\n", reg);

    unsigned char data[6] = {0, 0, 0, 0, 0, 0};
    if (read(i2cfd, data, 6) != 6) {
        printf("Error: I2C read failure for register data 0x%02X\n", reg);
        return (-1);
    }

    int16_t buf = ((int16_t)data[1] << 8) | data[0];
    if (VERBOSE) printf("Debug: Euler Orientation H: LSB [0x%02X] MSB [0x%02X] INT16 [%d]\n", data[0], data[1], buf);
    bnod_ptr->eul_head = (double)buf / 16.0;

    buf = ((int16_t)data[3] << 8) | data[2];
    if (VERBOSE) printf("Debug: Euler Orientation R: LSB [0x%02X] MSB [0x%02X] INT16 [%d]\n", data[2], data[3], buf);
    bnod_ptr->eul_roll = (double)buf / 16.0;

    buf = ((int16_t)data[5] << 8) | data[4];
    if (VERBOSE) printf("Debug: Euler Orientation P: LSB [0x%02X] MSB [0x%02X] INT16 [%d]\n", data[4], data[5], buf);
    bnod_ptr->eul_pitc = (double)buf / 16.0;
    return (0);
}

/* ------------------------------------------------------------ *
 * set_mode() - set the sensor operational mode register 0x3D   *
 * The modes cannot be switched over directly, first it needs   *
 * to be set to "config" mode before switching to the new mode. *
 * ------------------------------------------------------------ */
int BNO_set_mode(opmode_t newmode) {
    char data[2] = {0};
    data[0] = BNO055_OPR_MODE_ADDR;
    opmode_t oldmode = get_mode();

    if (oldmode == newmode)
        return (0);                        // if new mode is the same
    else if (oldmode > 0 && newmode > 0) { // switch to "config" first
        data[1] = 0x0;
        if (VERBOSE) printf("Debug: Write opr_mode: [0x%02X] to register [0x%02X]\n", data[1], data[0]);
        if (write(i2cfd, data, 2) != 2) {
            printf("Error: I2C write failure for register 0x%02X\n", data[0]);
            return (-1);
        }
        /* --------------------------------------------------------- *
         * switch time: any->config needs 7ms + small buffer = 10ms  *
         * --------------------------------------------------------- */
        usleep(10 * 1000);
    }

    data[1] = newmode;
    if (VERBOSE) printf("Debug: Write opr_mode: [0x%02X] to register [0x%02X]\n", data[1], data[0]);
    if (write(i2cfd, data, 2) != 2) {
        printf("Error: I2C write failure for register 0x%02X\n", data[0]);
        return (-1);
    }
    /* --------------------------------------------------------- *
     * switch time: config->any needs 19ms + small buffer = 25ms *
     * --------------------------------------------------------- */
    usleep(25 * 1000);

    if (get_mode() == newmode)
        return (0);
    else
        return (-1);
}

/* ------------------------------------------------------------ *
 * set_power() - set the sensor power mode in register 0x3E.    *
 * The power modes cannot be switched over directly, first the  *
 * ops mode needs to be "config"  to write the new power mode.  *
 * ------------------------------------------------------------ */
int BNO_set_power(power_t pwrmode) {
    char data[2] = {0};

    /* ------------------------------------------------------------ *
     * Check what operational mode we are in                        *
     * ------------------------------------------------------------ */
    opmode_t oldmode = get_mode();

    /* ------------------------------------------------------------ *
     * If ops mode wasn't config, switch to "CONFIG" mode first     *
     * ------------------------------------------------------------ */
    if (oldmode > 0) {
        data[0] = BNO055_OPR_MODE_ADDR;
        data[1] = 0x0;
        if (VERBOSE) printf("Debug: Write opr_mode: [0x%02X] to register [0x%02X]\n", data[1], data[0]);
        if (write(i2cfd, data, 2) != 2) {
            printf("Error: I2C write failure for register 0x%02X\n", data[0]);
            return (-1);
        }
        usleep(30 * 1000);
    } // now we are in config mode

    /* ------------------------------------------------------------ *
     * Set the new power mode                                       *
     * ------------------------------------------------------------ */
    data[0] = BNO055_PWR_MODE_ADDR;
    data[1] = pwrmode;
    if (VERBOSE) printf("Debug: Write opr_mode: [0x%02X] to register [0x%02X]\n", data[1], data[0]);
    if (write(i2cfd, data, 2) != 2) {
        printf("Error: I2C write failure for register 0x%02X\n", data[0]);
        return (-1);
    }
    usleep(30 * 1000);

    /* ------------------------------------------------------------ *
     * If ops mode wasn't config, switch back to original ops mode  *
     * ------------------------------------------------------------ */
    if (oldmode > 0) {
        data[0] = BNO055_OPR_MODE_ADDR;
        data[1] = oldmode;
        if (VERBOSE) printf("Debug: Write opr_mode: [0x%02X] to register [0x%02X]\n", data[1], data[0]);
        if (write(i2cfd, data, 2) != 2) {
            printf("Error: I2C write failure for register 0x%02X\n", data[0]);
            return (-1);
        }
        usleep(30 * 1000);
    } // now the previous mode is back

    if (get_power() == pwrmode)
        return (0);
    else
        return (-1);
}

/* ------------------------------------------------------------ *
 * get_sstat() returns the sensor sys status from register 0x39 *
 * ------------------------------------------------------------ */
int BNO_get_sstat() {
    int reg = BNO055_SYS_STAT_ADDR;
    if (write(i2cfd, &reg, 1) != 1) {
        printf("Error: I2C write failure for register 0x%02X\n", reg);
        return (-1);
    }

    unsigned int data = 0;
    if (read(i2cfd, &data, 1) != 1) {
        printf("Error: I2C read failure for register data 0x%02X\n", reg);
        return (-1);
    }

    if (VERBOSE) printf("Debug:  System Status: [0x%02X]\n", data);

    return (data);
}

/* ------------------------------------------------------------ *
 * print_sstat() - prints the sensor system status string from  *
 * the numeric value located in the sys_stat register 0x39      *
 * ------------------------------------------------------------ */
int BNO_print_sstat(int stat_code) {
    if (stat_code < 0 || stat_code > 6) return (-1);

    switch (stat_code) {
    case 0x00:
        printf("Idle\n");
        break;
    case 0x01:
        printf("System Error\n");
        break;
    case 0x02:
        printf("Initializing Peripherals\n");
        break;
    case 0x03:
        printf("System Initalization\n");
        break;
    case 0x04:
        printf("Executing Self-Test\n");
        break;
    case 0x05:
        printf("Sensor running with fusion algorithm\n");
        break;
    case 0x06:
        printf("Sensor running without fusion algorithm\n");
        break;
    }
    return (0);
}
