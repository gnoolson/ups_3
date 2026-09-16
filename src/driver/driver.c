#include "driver.h"
#include "../serial/serial.h"

driver_t* driver_new_and_setup(uint8_t pin) {
    driver_t* p_driver = (driver_t*)malloc(sizeof(driver_t));
    p_driver->pin = pin;
    p_driver->state = false;

    return p_driver;
}

void driver_delete(driver_t* p_driver) {
    free(p_driver);
}

void driver_begin(driver_t* p_driver) {
    pinMode(p_driver->pin, OUTPUT);
}

void driver_turn_on(driver_t* p_driver) {
    if (p_driver->state)
        return;

    p_driver->state = true;
    digitalWrite(p_driver->pin, p_driver->state);

    serial_print_text("Driver. Pin: ");
    serial_print_int(p_driver->pin);
    serial_println_text(" State: true");
}

void driver_turn_off(driver_t* p_driver) {
    if (!p_driver->state)
        return;

    p_driver->state = false;
    digitalWrite(p_driver->pin, p_driver->state);

    serial_print_text("Driver. Pin ");
    serial_print_int(p_driver->pin);
    serial_println_text(" State: false");
}

bool driver_is_on(driver_t* p_driver) {
    return p_driver->state;
}