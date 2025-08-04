/*********************************************************\
|                                                         |
| esc_firmware.ino                                        |
|                                                         |
| © 2025 Oscar Barbieri and Stefan Teodorescu.            |
|                                                         |
| This file contains the firmware for the ATTINY25/45/85  |
| microcontroller on the robot's main circuit board.      |
|                                                         |
| For proper compilation on the Arduino IDE, the          |
| following setup has to be used:                         |
|                                                         |
| 1. Install the ATTiny Board Package by David A Mellis.  |
| 2. Set board to "ATtiny25/45/85".                       |
| 3. Set clock to "Internal 8MHz".                        |
| 4. Use Arduino as ISP programmer.                       |
|                                                         |
| This program is small enough to be used on an ATTINY25. |
|                                                         |
\*********************************************************/

#include <USIWire.h> // NOLINT: Ignore this warning as the code will be compiled with Arduino IDE.

#define SLAVE_ADDRESS 0x08

#define MOT_A 3
#define MOT_B 4
#define TACH 1

#define PWM_FREQ 60 // Hz
#define PWM_PERIOD 1000 / PWM_FREQ

unsigned long tachometer_acc = 0;

char MOT_A_when_pwm_high = HIGH;
char MOT_A_when_pwm_low = HIGH;
char MOT_B_when_pwm_high = HIGH;
char MOT_B_when_pwm_low = HIGH;

unsigned int time_off = PWM_PERIOD / 2;

void setup() {
    pinMode(MOT_A, OUTPUT);
    pinMode(MOT_B, OUTPUT);

    cli(); // Disable global interrupts.
    PCMSK |= (1 << TACH);
    GIMSK |= (1 << PCIE);
    pinMode(TACH, INPUT_PULLUP);
    sei(); // Enable global interrupts.

    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
}

void loop() { // A low frequancy software PWM routine for the H-bridge IC.
    delay(time_off);
    digitalWrite(MOT_A, MOT_A_when_pwm_high);
    digitalWrite(MOT_B, MOT_B_when_pwm_high);
    delay(PWM_PERIOD - time_off);
    digitalWrite(MOT_A, MOT_A_when_pwm_low);
    digitalWrite(MOT_B, MOT_B_when_pwm_low);
}

ISR(INT0_vect) { // Event to handle a pulse has been recieved from the tachometer.
    tachometer_acc++;
}

void receiveData(int bytecount) { // Event when we recieve an I²C write.
    if (bytecount <= 0 || bytecount > 2) return;

    char cmd = Wire.read();                         // Read the command.
    char val = bytecount == 2 ? Wire.read() : 0x00; // Did we get a value along with the command?

    switch (cmd) {
    case 0x80: // Set motor speed (out of 255).
        time_off = PWM_PERIOD - PWM_PERIOD * val / 255;
        break;

    case 0x81: // Brake.
        MOT_A_when_pwm_high = HIGH;
        MOT_A_when_pwm_low = HIGH;
        MOT_B_when_pwm_high = HIGH;
        MOT_B_when_pwm_low = HIGH;
        break;

    case 0x82: // Turn motor forwards.
        MOT_A_when_pwm_high = HIGH;
        MOT_A_when_pwm_low = LOW;
        MOT_B_when_pwm_high = LOW;
        MOT_B_when_pwm_low = LOW;
        break;

    case 0x83: // Turn motor in reverse.
        MOT_A_when_pwm_high = LOW;
        MOT_A_when_pwm_low = LOW;
        MOT_B_when_pwm_high = HIGH;
        MOT_B_when_pwm_low = LOW;
        break;

    case 0x84: // Coast.
        MOT_A_when_pwm_high = LOW;
        MOT_A_when_pwm_low = LOW;
        MOT_B_when_pwm_high = LOW;
        MOT_B_when_pwm_low = LOW;
        break;

    case 0x85: // Reset tachometer.
        tachometer_acc = 0;
    }
}

void sendData() { // If we try to read from any I²C address, just return the tachometer counter.
    // Split tachometer counter into four bytes (little-endian; least significant byte first).
    char output[4];

    output[0] = tachometer_acc & 0xFF;
    output[1] = (tachometer_acc >> 8) & 0xFF;
    output[2] = (tachometer_acc >> 16) & 0xFF;
    output[3] = (tachometer_acc >> 24) & 0xFF;

    // Write to output.
    Wire.write(output, sizeof(output));
}
