#ifndef STOP_C
#define STOP_C
#include <pigpio.h> // NOLINT: This header exists on the target system. Ignore any warnings.
#include <unistd.h>

int main() {
    gpioPWM(13, 75);
    sleep(2);
    gpioTerminate();
}
#endif // STOP_C