#ifndef ESC_H
#define ESC_H

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define esc_ADDR 0x08 // The I2C address of the ESC controller.
#define esc_CHANNEL 1 // The i2c port on the SBC to be used.

int esc_set_speed(unsigned char speed);
int esc_brake();
int esc_drive(int direction);
int esc_coast();
int esc_tach_reset();
int esc_tach_read(unsigned long *counter);

#endif // ESC_H