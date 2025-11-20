#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

// (Main과 Cleaner Controller도 이 타입을 알아야 하므로 헤더에 정의)
typedef enum {
    ST_FORWARD,
    ST_TURN_LEFT,
    ST_TURN_RIGHT,
    ST_BACKWARD
} State;

State run_motor_controller();

#endif