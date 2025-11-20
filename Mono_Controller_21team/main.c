#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rvc_controller.h"
#include "hal_emulator.h"

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