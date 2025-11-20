#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> 

#include "hal_emulator.h"

void wait_tick(int ms) {
    Sleep(ms);
    
}

bool read_hardware_sensor() {
    // 20% 확률로 장애물/먼지 감지 (시뮬레이션) ------수정가능
    bool detected = (rand() % 10) < 2;
    return detected;
}

void write_hardware_motor(char* action) {
   // printf(" >> [MOTOR]: %s\n", action);
}

void write_hardware_cleaner(char* status) {
   // printf(" >> [CLEANER]: %s\n", status);
} 