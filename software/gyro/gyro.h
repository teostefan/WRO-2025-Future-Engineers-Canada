#ifndef GYRO_H
#define GYRO_H

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

extern char BNO055_addr[256] = "0x28";

#define VERBOSE 0
#define I2CBUS "/dev/i2c-1"
#define POWER_MODE_NORMAL 0x00
#define CALIB_BYTECOUNT 34
#define REGISTERMAP_END 0x7F

/* ------------------------------------------------------------ *
 * Page-0 registers with general confguration and data output   *
 * ------------------------------------------------------------ */
#define BNO055_CHIP_ID_ADDR 0x00

/* Mag data register */
#define BNO055_MAG_DATA_X_LSB_ADDR 0x0E
#define BNO055_MAG_DATA_X_MSB_ADDR 0x0F
#define BNO055_MAG_DATA_Y_LSB_ADDR 0x10
#define BNO055_MAG_DATA_Y_MSB_ADDR 0x11
#define BNO055_MAG_DATA_Z_LSB_ADDR 0x12
#define BNO055_MAG_DATA_Z_MSB_ADDR 0x13

/* Euler data registers */
#define BNO055_EULER_H_LSB_ADDR 0x1A
#define BNO055_EULER_H_MSB_ADDR 0x1B
#define BNO055_EULER_R_LSB_ADDR 0x1C
#define BNO055_EULER_R_MSB_ADDR 0x1D
#define BNO055_EULER_P_LSB_ADDR 0x1E
#define BNO055_EULER_P_MSB_ADDR 0x1F

/* Status registers */
#define BNO055_SYS_STAT_ADDR 0x39

/* Mode registers */
#define BNO055_OPR_MODE_ADDR 0x3D
#define BNO055_PWR_MODE_ADDR 0x3E

#define BNO055_SYS_TRIGGER_ADDR 0x3F

/* ------------------------------------------------------------ *
 * BNO055 measurement data structs. Data gets filled in based   *
 * on the sensor component type that was requested for reading. *
 * ------------------------------------------------------------ */
struct bnomag {
    double mdata_x; // magnetometer data, X-axis
    double mdata_y; // magnetometer data, Y-axis
    double mdata_z; // magnetometer data, Z-axis
};
struct bnoeul {
    double eul_head; // Euler heading data
    double eul_roll; // Euler roll data
    double eul_pitc; // Euler picth data
};

/* ------------------------------------------------------------ *
 * Operations and power mode, name to value translation         *
 * ------------------------------------------------------------ */
typedef enum {
    config = 0x00,
    acconly = 0x01,
    magonly = 0x02,
    gyronly = 0x03,
    accmag = 0x04,
    accgyro = 0x05,
    maggyro = 0x06,
    amg = 0x07,
    imu = 0x08,
    compass = 0x09,
    m4g = 0x0A,
    ndof = 0x0B,
    ndof_fmc = 0x0C
} opmode_t;

typedef enum {
    normal = 0x00,
    low = 0x01,
    suspend = 0x02
} power_t;

/* ------------------------------------------------------------ *
 * external function prototypes for I2C bus communication code  *
 * ------------------------------------------------------------ */
extern void BNO_init_i2cbus(char *, char *);                                             // get the I2C bus file handle
extern int BNO_get_mag(struct bnomag *);                                                 // read magnetometer data
extern int BNO_get_eul(struct bnoeul *, int startDirection, int direction, int stretch); // read euler orientation
extern int BNO_set_mode(opmode_t);                                                       // set the sensor ops mode
extern int BNO_set_power(power_t);                                                       // set the sensor power mode
extern int BNO_get_sstat();                                                              // get system status code
extern int BNO_print_sstat(int);                                                         // print system status string
extern int BNO_reset();                                                                  // reset the sensor

#endif // GYRO_H