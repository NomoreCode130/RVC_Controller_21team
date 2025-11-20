#include <stdio.h>
#include "cleaner_controller.h"
#include "rvc_sensors.h"
#include "rvc_actions.h"

const char* CLEANER_CMD_STR[] = {
    "OFF  ",
    "ON   ",
    "POWER-UP"
};


void run_cleaner_controller(State motor_status) {
    // 1. Input (변수명 통일)
    bool dust_existence = get_dust_existence();
    CleanerCmd cleaner_cmd = CMD_OFF;

    // 2. Process
    // 전진 중일 때만 켜고, 먼지 있으면 파워업
    if (motor_status == ST_FORWARD) {
        if (dust_existence) cleaner_cmd = CMD_UP;
        else                cleaner_cmd = CMD_ON;
    }
    else {
        // 회전하거나 후진 중이면 끔
        cleaner_cmd = CMD_OFF;
    }

    // 3. Output (Action)
    cleaner_control(cleaner_cmd);

    printf("[CLEANER] Dust: %s | Cmd: %s\n",
         dust_existence ? "Yes" : "No ",
        CLEANER_CMD_STR[cleaner_cmd]);
}