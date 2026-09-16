#include "sensor_220v.h"
#include "../serial/serial.h"

#define SENSOR_220V_ZERO 20

void IRAM_ATTR handle_alert(void* arg) {
    sensor_220v_t* p_sensor = (sensor_220v_t*)arg;
    p_sensor->zero_counter = p_sensor->zero_counter + 1;
    p_sensor->start_timer = true;
}

sensor_220v_t* sensor_220v_new_and_setup(uint8_t alert_pin) {
    sensor_220v_t* p_sensor = (sensor_220v_t*)malloc(sizeof(sensor_220v_t));
    p_sensor->zero_counter = 0;
    p_sensor->alert_pin = alert_pin;
    p_sensor->has_voltage = false;
    p_sensor->p_timer = gnl_timer_new_and_setup(50, false);
    p_sensor->p_zero_counter_timer = gnl_timer_new_and_setup(100, true);
    gnl_timer_start(p_sensor->p_zero_counter_timer);

    return p_sensor;
}

void sensor_220v_delete(sensor_220v_t* p_sensor) {
    gnl_timer_delete(p_sensor->p_timer);
    gnl_timer_delete(p_sensor->p_zero_counter_timer);
    free(p_sensor);
}

void sensor_220v_update(sensor_220v_t* p_sensor) {
    if (p_sensor->zero_counter >= SENSOR_220V_ZERO) {
        p_sensor->zero_counter = SENSOR_220V_ZERO;

        if (!p_sensor->has_voltage) {
            serial_print_text("S220V. Pin: ");
            serial_print_int(p_sensor->alert_pin);
            serial_println_text(" Voltage: true");
        }
        p_sensor->has_voltage = true;
    }

    if (gnl_timer_check(p_sensor->p_zero_counter_timer) && p_sensor->zero_counter < SENSOR_220V_ZERO)
        p_sensor->zero_counter = 0;

    if (p_sensor->start_timer) {
        p_sensor->start_timer = false;
        gnl_timer_start(p_sensor->p_timer);
    }

    if (gnl_timer_check(p_sensor->p_timer)) {
        if (p_sensor->has_voltage) {
            serial_print_text("S220V. Pin: ");
            serial_print_int(p_sensor->alert_pin);
            serial_println_text(" Voltage: false");
        }

        p_sensor->has_voltage = false;
        p_sensor->zero_counter = 0;
    }
}

void sensor_220v_begin(sensor_220v_t* p_sensor) {
    pinMode(p_sensor->alert_pin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(p_sensor->alert_pin), handle_alert, p_sensor, FALLING);
}

bool sensor_220v_has_voltage(sensor_220v_t* p_sensor) {
    return p_sensor->has_voltage;
}
