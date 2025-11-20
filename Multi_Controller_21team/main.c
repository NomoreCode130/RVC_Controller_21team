#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "motor_controller.h"
#include "cleaner_controller.h"
#include "hal_emulator.h"

int main() {
    srand(time(NULL));
    printf("=== RVC Multi-Controller System Started ===\n\n");
    printf("=== Press Ctrl+C to Stop ===\n\n");

    State motor_status = ST_FORWARD;

    while (1) {
        // 1. Motor Controller 실행 & 상태 받아오기
        motor_status = run_motor_controller();

        // 2. Cleaner Controller 실행 & 상태 넘겨주기
        run_cleaner_controller(motor_status);

        // 3. Tick 대기 (1초)
        wait_tick(1000);
    }

    return 0;
}