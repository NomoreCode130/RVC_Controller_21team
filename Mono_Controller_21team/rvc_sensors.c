#include "rvc_sensors.h"
#include "hal_emulator.h"



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