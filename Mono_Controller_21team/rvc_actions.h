#ifndef RVC_ACTIONS_H
#define RVC_ACTIONS_H

#include <stdbool.h>

// Cleaner Command
typedef enum { CMD_OFF, CMD_ON, CMD_UP } CleanerCmd;

void move_forward(bool enable_flag);
void turn_left(bool trigger_signal);
void turn_right(bool trigger_signal);
void move_backward(bool enable_flag);
void cleaner_control(CleanerCmd cleaner_cmd);

#endif