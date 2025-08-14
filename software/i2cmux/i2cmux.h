#ifndef I2CMUX_H
#define I2CMUX_H

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define i2cmux_ADDR 0x70 // The I2C address of the multiplexer.
#define i2cmux_CHANNEL 1 // The i2c port on the SBC to be used.

int i2cmux_switch(int channel);

#endif // I2CMUX_H