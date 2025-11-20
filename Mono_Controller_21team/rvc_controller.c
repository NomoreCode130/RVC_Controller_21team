#include <stdio.h>
#include "rvc_controller.h"
#include "rvc_sensors.h"
#include "rvc_actions.h"

// FSM 상태
typedef enum {
    ST_FORWARD,
    ST_TURN_LEFT,
    ST_TURN_RIGHT,
    ST_BACKWARD
} State;

const char* STATE_STRINGS[] = {
    "FORWARD   ",
    "TURN_LEFT ",
    "TURN_RIGHT",
    "BACKWARD  "
};


static State current_state = ST_FORWARD;
static int tick_counter = 0; // 회전 시간(5틱) 체크용

void main_controller() {
    // 1. Input
    int obstacle_location = get_obstacle_location(); // L/F/R bits
    bool dust_existence = get_dust_existence();

    // 비트 디코딩 (가독성용)
    bool l = (obstacle_location & 4);   //100 -> True면 Left 막힘
    bool f = (obstacle_location & 2);   //010
    bool r = (obstacle_location & 1);   //011

    // 2. Process
    switch (current_state) {
    case ST_FORWARD:
        // 청소 제어
        if (dust_existence) cleaner_control(CMD_UP);
        else cleaner_control(CMD_ON);

        // Action: 모터 제어 (전진)
        move_forward(true);

        // Transition Logic
        if (f && l && r) { // 모두 막힘 -> 후진
            current_state = ST_BACKWARD;
            tick_counter = 0;
        }
        else if (f && !l && r) { // 전방+우측 막힘 -> 좌회전 우선
            current_state = ST_TURN_LEFT;
            tick_counter = 0;
        }
        else if (f && !r) { // 전방만 막힘 or 전방+좌측 -> 우회전
            current_state = ST_TURN_RIGHT;
            tick_counter = 0;
        }
        break;

    case ST_TURN_LEFT:
        cleaner_control(CMD_OFF); // 회전 중 청소 끔
        turn_left(true);

        tick_counter++;
        if (tick_counter >= 5) { // 5 Tick 후 복귀
            current_state = ST_FORWARD;
            tick_counter = 0;
        }
        break;

    case ST_TURN_RIGHT:
        cleaner_control(CMD_OFF);
        turn_right(true);

        tick_counter++;
        if (tick_counter >= 5) {
            current_state = ST_FORWARD;
            tick_counter = 0;
        }
        break;

    case ST_BACKWARD:
        cleaner_control(CMD_OFF);
        move_backward(true);

        tick_counter++;
        if (tick_counter >= 5) { // 5틱 후 회전
            if (!r) {  //우회전 우선
                current_state = ST_TURN_RIGHT;
                tick_counter = 0;
            }
            else { //좌회전
                current_state = ST_TURN_LEFT;
                tick_counter = 0;
            }
        }
        break;
    }


    printf("\n---------------- [TICK] ----------------\n");
    printf("상태: %s | 장애물: [%s][%s][%s] | 먼지: %s | 틱: %d\n",
        STATE_STRINGS[current_state],
        l ? "BLK" : "_좌_",                // 막히면 BLK, 뚫리면 ___
        f ? "BLK" : "_전_",
        r ? "BLK" : "_우_",
        dust_existence ? "Yes" : "No",
        tick_counter);
}