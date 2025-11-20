#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h> 

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

typedef enum { CMD_OFF, CMD_ON, CMD_UP } CleanerCmd;    //Cleaner command

static State current_state = ST_FORWARD;
static int tick_counter = 0; // 회전 시간(5틱) 체크용

// <<HARDWARE EMULATOR>>
void wait_tick(int ms) { 
    Sleep(ms);
}

bool read_hardware_sensor() {
    // 20% 확률로 장애물/먼지 감지 (시뮬레이션) ------수정가능
    bool detected = (rand() % 10) < 2;
    return detected;
}

void write_hardware_motor(char* action) { printf(" >> [MOTOR]: %s\n\n", action);}

void write_hardware_cleaner(char* status) { printf(" >> [CLEANER]: %s\n", status);}

//~HARDWARE EMULATOR

//PRINTING SYSTEM STATUS(함수)

void print_system_status(State current_state,
    int obstacle_location,
    bool dust_existence,
    int tick_counter)
{
    bool l = obstacle_location & 4;
    bool f = obstacle_location & 2;
    bool r = obstacle_location & 1;

    printf("\n============= [SYSTEM STATUS] =============\n");
    printf("상태         : %s\n", STATE_STRINGS[current_state]);
    printf("장애물       : L[%s]  F[%s]  R[%s]\n",
        l ? "BLOCK" : " OPEN ",
        f ? "BLOCK" : " OPEN ",
        r ? "BLOCK" : " OPEN ");
    printf("먼지         : %s\n", dust_existence ? "DETECTED" : "NONE     ");
    printf("틱           : %d\n", tick_counter);
    printf("===========================================\n\n");
}





// <<SENSOR MODULE>>
// 센서 인터페이스
bool front_sensor_interface() { return read_hardware_sensor(); }
bool left_sensor_interface() { return read_hardware_sensor(); }
bool right_sensor_interface() { return read_hardware_sensor(); }
bool dust_sensor_interface() { return read_hardware_sensor(); }

// 중간 모듈
int get_obstacle_location() {
    bool f = front_sensor_interface();
    bool l = left_sensor_interface();
    bool r = right_sensor_interface();

    // 3-bit encoding: Left(4) | Front(2) | Right(1)
    // 예: 111(7) = 모두 막힘, 010(2) = front만 막힘
    int location = (l << 2) | (f << 1) | r; //비트 이동 연산자 사용
    return location;
}

bool get_dust_existence() {
    return dust_sensor_interface();
}
//~SENSOR MODULE


//<<ACTION MODULE>>
void move_forward(bool enable_flag) {
    if (enable_flag) write_hardware_motor("Move Forward");
}

void turn_left(bool trigger_signal) {
    if (trigger_signal) write_hardware_motor("Turn Left");
}

void turn_right(bool trigger_signal) {
    if (trigger_signal) write_hardware_motor("Turn Right");
}

void move_backward(bool enable_flag) {
    if (enable_flag) write_hardware_motor("Move Backward");
}

void cleaner_control(CleanerCmd cleaner_cmd) {
    switch (cleaner_cmd) {
    case CMD_OFF:   write_hardware_cleaner("OFF"); break;
    case CMD_ON:    write_hardware_cleaner("ON"); break;
    case CMD_UP: write_hardware_cleaner("POWER-UP"); break;
    }
}
//~ACTION MODULE

//CONTROLLER MODULE
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
    print_system_status(current_state, obstacle_location, dust_existence, tick_counter);

}
//~CONTROLLER MODULE



int main() {
    srand(time(NULL));

    printf("=== RVC Mono-Controller System Started ===\n");
    printf("=== Press Ctrl+C to Stop ===\n\n");

    while (1) {
        // 1. Controller 호출 (Tick 단위 실행)
        main_controller();

        // 2. Tick 시뮬레이션
        wait_tick(1000);    //더 느리게 -> 숫자 크게
    }

    return 0;
}