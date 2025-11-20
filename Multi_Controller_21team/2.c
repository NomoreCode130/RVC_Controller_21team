#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h> 

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

//Cleaner Command
typedef enum { CMD_OFF, CMD_ON, CMD_UP } CleanerCmd;

const char* CLEANER_CMD_STR[] = {
    "OFF      ",
    "ON       ",
    "POWER-UP "
};

static State current_state = ST_FORWARD;
static int   tick_counter = 0;      // 회전 시간(5틱) 체크용


//print_system_status()를 위한 상태저장변수
static int   last_obstacle_loc = 0;      // 마지막 장애물 비트
static bool  last_dust_existence = false;  // 마지막 먼지 존재 여부
static CleanerCmd last_cleaner_cmd = CMD_OFF;


// <<HARDWARE EMULATOR>>
void wait_tick(int ms) {
    Sleep(ms);
}

bool read_hardware_sensor() {
    // 20% 확률로 장애물/먼지 감지 (시뮬레이션) ------수정가능
    bool detected = (rand() % 10) < 2;
    return detected;
}

void write_hardware_motor(char* action) {
    printf(" >> [MOTOR]   : %s\n", action);
}

void write_hardware_cleaner(char* status) {
    printf(" >> [CLEANER] : %s\n", status);
}
//~HARDWARE EMULATOR

// printing system status
void print_system_status(State state,
    int obstacle_location,
    bool dust_existence,
    int tick,
    CleanerCmd cleaner_cmd)
{
    bool l = obstacle_location & 4;   // 100
    bool f = obstacle_location & 2;   // 010
    bool r = obstacle_location & 1;   // 001

    printf("\n============= [SYSTEM STATUS] =============\n");
    printf("상태         : %s\n", STATE_STRINGS[state]);
    printf("장애물       : L[%s]  F[%s]  R[%s]\n",
        l ? "BLOCK" : " OPEN ",
        f ? "BLOCK" : " OPEN ",
        r ? "BLOCK" : " OPEN ");
    printf("먼지         : %s\n", dust_existence ? "DETECTED" : "NONE     ");
    printf("클리너      : %s\n", CLEANER_CMD_STR[cleaner_cmd]);
    printf("틱           : %d\n", tick);
    printf("===========================================\n");
    printf("\n");
}
//~

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
    int location = (l << 2) | (f << 1) | r; // 비트 이동 연산자 사용

    // 상태 출력용으로 저장
    last_obstacle_loc = location;
    return location;
}

bool get_dust_existence() {
    bool dust = dust_sensor_interface();
    last_dust_existence = dust;
    return dust;
}
// ~SENSOR MODULE

// <<ACTION MODULE>>
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
    case CMD_OFF: write_hardware_cleaner("OFF");       break;
    case CMD_ON:  write_hardware_cleaner("ON");        break;
    case CMD_UP:  write_hardware_cleaner("POWER-UP");  break;
    }
}
//~ACTION MODULE

// <<MOTOR CONTROLLER MODULE>>
State run_motor_controller() {
    // 1. Input
    int obstacle_location = get_obstacle_location(); // L/F/R bits

    // 비트 디코딩 (가독성용)
    bool l = (obstacle_location & 4);   // Left 막힘 여부
    bool f = (obstacle_location & 2);   // Front
    bool r = (obstacle_location & 1);   // Right

    // 2. Process (FSM)
    switch (current_state) {
    case ST_FORWARD:
        // Action: 모터 제어 (전진)
        move_forward(true);

        // Transition Logic (장애물만 보고 상태 변경)
        if (f && l && r) {              // 모두 막힘 -> 후진
            current_state = ST_BACKWARD;
            tick_counter = 0;
        }
        else if (f && !l && r) {        // 전방+우측 막힘 -> 좌회전 우선
            current_state = ST_TURN_LEFT;
            tick_counter = 0;
        }
        else if (f && !r) {             // 전방만 막힘 or 전방+좌측 -> 우회전
            current_state = ST_TURN_RIGHT;
            tick_counter = 0;
        }
        break;

    case ST_TURN_LEFT:
        turn_left(true);
        tick_counter++;
        if (tick_counter >= 5) {        // 5 Tick 후 복귀
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
        if (tick_counter >= 5) {        // 5틱 후 회전
            if (!r) {                   // 우회전 우선
                current_state = ST_TURN_RIGHT;
                tick_counter = 0;
            }
            else {                      // 좌회전
                current_state = ST_TURN_LEFT;
                tick_counter = 0;
            }
        }
        break;
    }

    // 디버깅용 printf는 여기서 안 찍는다
    return current_state;
}
// ~MOTOR CONTROLLER MODULE

// <<CLEANER CONTROLLER MODULE
void run_cleaner_controller(State motor_status) {
    // 1. Input
    bool dust_existence = get_dust_existence();
    CleanerCmd cleaner_cmd = CMD_OFF;

    // 2. Process
    // 모터가 전진 중일 때만 클리너 동작, 먼지 있으면 파워업
    if (motor_status == ST_FORWARD) {
        if (dust_existence) cleaner_cmd = CMD_UP;
        else                cleaner_cmd = CMD_ON;
    }
    else {
        // 회전하거나 후진 중이면 끔
        cleaner_cmd = CMD_OFF;
    }

    // 상태 출력용으로 저장
    last_cleaner_cmd = cleaner_cmd;

    // 3. Output
    cleaner_control(cleaner_cmd);
}
// ~CLEANER CONTROLLER MODULE


int main() {
    srand((unsigned int)time(NULL));
    printf("=== RVC Multi-Controller System Started ===\n");
    printf("=== Press Ctrl+C to Stop ===\n\n");

 
    while (1) {
        // 1. Motor Controller 실행 & 상태 받아오기
        State motor_status = run_motor_controller();

        // 2. Cleaner Controller 실행 & 상태 넘겨주기
        run_cleaner_controller(motor_status);

        print_system_status(current_state,
            last_obstacle_loc,
            last_dust_existence,
            tick_counter,
            last_cleaner_cmd);

        // 3. Tick 대기 (1초)
        wait_tick(1000);
    }

    return 0;
}