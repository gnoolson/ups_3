#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include <gnl_timer.h>

typedef struct {
    uint8_t pin;
	uint8_t counter;
	bool state;
	gnl_timer_t* p_timer;
} buzzer_t;

/*
 * 
 * 
 * */
buzzer_t* buzzer_new_and_setup(uint8_t pin);
void buzzer_delete(buzzer_t* p_buzzer);
void buzzer_begin(buzzer_t* p_buzzer);
void buzzer_upload(buzzer_t* p_buzzer);
void buzzer_beep(buzzer_t* p_buzzer, uint8_t quantity, bool slow);

#endif