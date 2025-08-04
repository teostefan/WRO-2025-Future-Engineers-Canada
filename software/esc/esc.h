#ifndef ESC_H
#define ESC_H

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define esc_ADDR 0x08                 // The I2C address of the ESC controller.
#define esc_CHANNEL 1                 // The i2c port on the SBC to be used.
#define esc_GPIO_PIN 23               // The GPIO pin used for enabling/disabling the ESC.
#define esc_STEERING_SERVO_PIN 1      // The PWM pin used for steering servo control. (PWM 1 = GPIO 13)
#define esc_STEERING_SERVO_OFFSET -18 // The offset for the steering servo angle.
#define esc_STEERING_SERVO_MIN 45     // The minimum angle for the steering servo.
#define esc_STEERING_SERVO_MAX -45    // The maximum angle for the steering servo

int esc_enable();
int esc_disable();
int esc_set_speed(unsigned char speed);
int esc_brake();
int esc_drive(int direction);
int esc_coast();
int esc_tach_reset();
int esc_tach_read(unsigned long *counter);
int esc_servo_steer(int angle);
int esc_servo_stop();

#endif // ESC_H