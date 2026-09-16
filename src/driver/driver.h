#ifndef DRIVER_H
#define DRIVER_H

#include <Arduino.h>

typedef struct {
    bool state;
    uint8_t pin;
} driver_t;

/*
 * 
 * 
 * */
driver_t* driver_new_and_setup(uint8_t pin);
void driver_delete(driver_t* p_driver);
void driver_begin(driver_t* p_driver);
void driver_turn_on(driver_t* p_driver);
void driver_turn_off(driver_t* p_driver);
bool driver_is_on(driver_t* p_driver);

#endif