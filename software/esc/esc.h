#ifndef ESC_H
#define ESC_H

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <pigpio.h> // NOLINT: This header exists on the target system. Ignore any warnings.
#include <stdio.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define esc_ADDR 0x08                // The I2C address of the ESC controller.
#define esc_CHANNEL 1                // The i2c port on the SBC to be used.
#define esc_GPIO_PIN 23              // The GPIO pin used for enabling/disabling the ESC.
#define esc_STEERING_SERVO_PIN 13    // The PWM pin used for steering servo control.
#define esc_STEERING_SERVO_OFFSET 18 // The offset for the steering servo angle.
#define esc_STEERING_SERVO_MIN -50   // The minimum angle in gradians for the steering servo.
#define esc_STEERING_SERVO_MAX 50    // The maximum angle in gradians for the steering servo

int esc_enable();
int esc_disable();
int esc_drive(int direction, unsigned char speed);
int esc_brake();
int esc_coast();
int esc_servo_init();
int esc_servo_steer(int angle);
int esc_servo_uninit();

#endif // ESC_H