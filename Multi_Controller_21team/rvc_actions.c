#include "rvc_actions.h"
#include "hal_emulator.h"

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