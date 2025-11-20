/* src/motor_controller.c */
#include <stdio.h>
#include "motor_controller.h"
#include "rvc_sensors.h"
#include "rvc_actions.h"

static State current_state = ST_FORWARD;
static int tick_counter = 0;

const char* STATE_STRINGS[] = {
    "FORWARD   ",
    "TURN_LEFT ",
    "TURN_RIGHT",
    "BACKWARD  "
};

State run_motor_controller() {
    // 1. Input
    int obstacle_location = get_obstacle_location();

    // 비트 디코딩 F(4)|L(2)|R(1)
    bool f = (obstacle_location & 4);
    bool l = (obstacle_location & 2);
    bool r = (obstacle_location & 1);

    // 2. Process
    switch (current_state) {
    case ST_FORWARD:
        move_forward(true);

        if (f && l && r) {
            current_state = ST_BACKWARD;
            tick_counter = 0;
        }
        else if (f && !l && r) {
            current_state = ST_TURN_LEFT;
            tick_counter = 0;
        }
        else if (f && !r) {
            current_state = ST_TURN_RIGHT;
            tick_counter = 0;
        }
        break;

    case ST_TURN_LEFT:
        turn_left(true);
        tick_counter++;
        if (tick_counter >= 5) {
            current_state = ST_FORWARD;
            tick_counter = 0;
        }
        break;

    case ST_TURN_RIGHT:
        turn_right(true);
        tick_counter++;
        if (tick_counter >= 5) {
            current_state = ST_FORWARD;
            tick_counter = 0;
        }
        break;

    case ST_BACKWARD:
        move_backward(true);
        tick_counter++;
        if (tick_counter >= 5) {
            // 우회전 우선 로직 (Mono 때 작성한 논리 그대로)
            if (!r) current_state = ST_TURN_RIGHT;
            else current_state = ST_TURN_LEFT;

            tick_counter = 0;
        }
        break;
    }

    printf("\n---------------- [TICK] ----------------\n");
    printf("[MOTOR] 상태: %s | 장애물: [%s][%s][%s] | 틱: %d\n",
        STATE_STRINGS[current_state],
        l ? "BLK" : "_좌_",  // 막히면 BLK, 뚫리면 ___
        f ? "BLK" : "_전_",
        r ? "BLK" : "_우_",
        tick_counter);

    // 3. Output
    return current_state;
}
