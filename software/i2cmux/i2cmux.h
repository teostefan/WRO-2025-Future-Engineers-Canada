#ifndef I2CMUX_H
#define I2CMUX_H

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define i2cmux_ADDR 0x70

int i2cmux_switch(int iChan, int channel);

#endif // I2CMUX_H