/*********************************************************\
|                                                         |
| esc.ino                                                 |
|                                                         |
| © 2025 Oscar Barbieri and Stefan Teodorescu.            |
|                                                         |
| This file contains the firmware for the ATTINY25/45/85  |
| microcontroller on the robot's main circuit board.      |
|                                                         |
| For proper compilation on the Arduino IDE, the          |
| following setup has to be used:                         |
|                                                         |
| 1. Copy just this file into an empty directory named    |
|    "esc" without the other C files found in the parent  |
|    directory. Open the file using the Arduino IDE v2.   |
| 2. Install the ATTiny Board Package by David A Mellis.  |
| 3. Set board to "ATtiny25/45/85".                       |
| 4. Set clock to "Internal 8MHz".                        |
| 5. Use Arduino as ISP programmer.                       |
|                                                         |
| This program is small enough to be used on an ATTINY25. |
|                                                         |
\*********************************************************/

#include <USIWire.h> // NOLINT: Ignore this warning as the code will be compiled with Arduino IDE.

#define HI_NIBBLE(b) (((b) >> 4) & 0x0F) // Get upper nibble from byte. Ex.: 0x36 -> 0x03
#define LO_NIBBLE(b) ((b) & 0x0F)        // Get lower nibble from byte. Ex.: 0x36 -> 0x06.
#define DBL_NIBBLE(n) ((n << 4) | n)     // Duplicate a nibble to form a byte. Ex.: 0x03 -> 0x33.

#define SLAVE_ADDRESS 0x08

#define MOT_A 3
#define MOT_B 4

void setup() {
    pinMode(MOT_A, OUTPUT);
    pinMode(MOT_B, OUTPUT);

    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
    // The next section was inspired from http://www.technoblogy.com/show?LE0=:
    TCCR1 = 1 << CTC1 | 1 << PWM1A | 3 << COM1A0 | 7 << CS10;
    GTCCR = 1 << PWM1B | 3 << COM1B0;
    // Interrupts on OC1A match and overflow
    TIMSK = TIMSK | 1 << OCIE1A | 1 << TOIE1;
}

ISR(TIMER1_COMPA_vect) {
    if (!bitRead(TIFR, TOV1)) bitSet(PORTB, MOT_A);
}

ISR(TIMER1_OVF_vect) {
    bitClear(PORTB, MOT_A);
}

void loop() {
}

void receiveData(int bytecount) { // Event when an I²C write is recieved.
    // Control byte format is as follows: xxxxyyyy where xxxx and yyyy are nibbles from 0 to 15 representing
    // the duty cycle for driving the H-bridge channels IN1 (xxxx) and IN2 (yyyy).
    if (bytecount != 1) return;

    char bytein = Wire.read();
    OCR1A = DBL_NIBBLE(LO_NIBBLE(bytein));
    OCR1B = DBL_NIBBLE(HI_NIBBLE(bytein));
}

void sendData() { // There is no I²C registers to be read from.
    Wire.write(0x00);
}
