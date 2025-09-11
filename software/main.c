#ifndef MAIN_C
#define MAIN_C

#include "globals.h"
#include "manoeuvres.c"

int main() {
    // calibrate sensors
    popen("i2cdetect -y 1", "r");
    for (int i = 0; i < 4; i++) {
        i2cmux_switch(i);
        usleep(50000); // 50ms
        tofInit(1, 0x29, 1);
        usleep(50000); // 50ms
    }

    BNO_init_i2cbus();
    usleep(50000); // 50ms

    do {
        if (BNO_get_eul(&angles) == -1) {
            printf("An error occurred.");
        }
        printf("Zero Heading = %f degrees ", angles.eul_head);
        zeroHeading = angles.eul_head;
        usleep(50000); // 50ms
    } while (zeroHeading < 1 || zeroHeading > 360);

    esc_enable();
    esc_servo_init();
    esc_servo_steer(0);
    esc_coast();

    direction = COUNTERCLOCKWISE;
    stretch = 0;

    out_out();
    out_blind_turn();

    for (int i = 1; i < 4; i++) {
        stretch = i % 4;

        if ((map[stretch] == GREEN_GREEN && direction == CLOCKWISE) || (map[stretch] == RED_RED && direction == COUNTERCLOCKWISE)) {
            out_out();
            out_blind_turn();
        } else if ((map[stretch] == GREEN_RED && direction == CLOCKWISE) || (map[stretch] == RED_GREEN && direction == COUNTERCLOCKWISE)) {
            out_in();
            in_blind_turn();
        } else if ((map[stretch] == RED_GREEN && direction == CLOCKWISE) || (map[stretch] == GREEN_RED && direction == COUNTERCLOCKWISE)) {
            in_out();
            out_blind_turn();
        } else if ((map[stretch] == RED_RED && direction == CLOCKWISE) || (map[stretch] == GREEN_GREEN && direction == COUNTERCLOCKWISE)) {
            in_in();
            in_blind_turn();
        }
    }

    for (int i = 0; i < 8; i++) {
        stretch = i % 4;
        next_stretch = (i + 1) % 4;

        if ((map[stretch] == GREEN_GREEN && direction == CLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN)) || (map[stretch] == RED_RED && direction == COUNTERCLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED))) {
            out_out();
            out_in_turn();
        } else if ((map[stretch] == GREEN_GREEN && direction == CLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED)) || (map[stretch] == RED_RED && direction == COUNTERCLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN))) {
            out_out();
            out_out_turn();
        } else if ((map[stretch] == GREEN_RED && direction == CLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN)) || (map[stretch] == RED_GREEN && direction == COUNTERCLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED))) {
            out_in();
            in_in_turn();
        } else if ((map[stretch] == GREEN_RED && direction == CLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED)) || (map[stretch] == RED_GREEN && direction == COUNTERCLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN))) {
            out_in();
            in_out_turn();
        } else if ((map[stretch] == RED_GREEN && direction == CLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN)) || (map[stretch] == GREEN_RED && direction == COUNTERCLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED))) {
            in_out();
            out_in_turn();
        } else if ((map[stretch] == RED_GREEN && direction == CLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED)) || (map[stretch] == GREEN_RED && direction == COUNTERCLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN))) {
            in_out();
            out_out_turn();
        } else if ((map[stretch] == RED_RED && direction == CLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN)) || (map[stretch] == GREEN_GREEN && direction == COUNTERCLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED))) {
            in_in();
            in_in_turn();
        } else if ((map[stretch] == RED_RED && direction == CLOCKWISE && (map[next_stretch] == GREEN_GREEN || map[next_stretch] == GREEN_RED)) || (map[stretch] == GREEN_GREEN && direction == COUNTERCLOCKWISE && (map[next_stretch] == RED_RED || map[next_stretch] == RED_GREEN))) {
            in_in();
            in_out_turn();
        }
    }

    /*

    do {
        i2cmux_switch(FRONT_MUX);
        usleep(50000); // 50ms
        int frontDistance = tofReadDistance();
        usleep(50000); // 50ms
        i2cmux_switch(REAR_MUX);
        usleep(50000); // 50ms
        int rearDistance = tofReadDistance();
        usleep(50000); // 50ms

        if (frontDistance <= SEE_WALL_DISTANCE && rearDistance <= SEE_WALL_DISTANCE) {
            challenge = challenge;
        } else if (frontDistance >= SEE_WALL_DISTANCE && rearDistance >= SEE_WALL_DISTANCE) {
            challenge = COUNTERCLOCKWISE;
        }
    } while ((frontDistance > SEE_WALL_DISTANCE && rearDistance < SEE_WALL_DISTANCE) || (frontDistance <= SEE_WALL_DISTANCE && rearDistance >= SEE_WALL_DISTANCE));

    if (challenge == OPEN) {
        open_challenge(); // see manoeuvres.c for more details
    } else if (challenge == OBSTACLE) {
        obstacle_challenge(); // see manoeuvres.c for more details
    }

    */

    printf("stopping");

    esc_coast();
    sleep(2);
    esc_servo_steer(0);
    sleep(2);
    esc_disable();
    esc_servo_uninit();

    printf("\nthe end.\n");

    return 0;
}

#endif // MAIN_C
