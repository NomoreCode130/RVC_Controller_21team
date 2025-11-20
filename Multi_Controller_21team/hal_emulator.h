#ifndef HAL_EMULATOR_H
#define HAL_EMULATOR_H

#include <stdbool.h>

//하드웨어 에뮬레이션을 위해서(랜덤값 출력)

// 시뮬레이션을 위한 Tick 대기 (Sleep)
void wait_tick(int ms);
// 랜덤 센서값 생성 (True/False)
bool read_hardware_sensor();
// 모터/청소기 동작 콘솔 출력
//void write_hardware_motor(char* action);
//void write_hardware_cleaner(char* status);


#endif