#include "buzzer.h"

#define BU_BEEP_TIMER 250
#define BU_SLOW_BEEP_TIMER 2000
#define BU_ANALOG_VALUE 128

buzzer_t* buzzer_new_and_setup(uint8_t pin) {
    buzzer_t* p_buzzer = (buzzer_t*)malloc(sizeof(buzzer_t));
    p_buzzer->pin = pin;
    p_buzzer->counter = 0;
    p_buzzer->state = false;
    p_buzzer->p_timer = gnl_timer_new_and_setup(0, false);
    return p_buzzer;
}

void buzzer_delete(buzzer_t* p_buzzer) {
    free(p_buzzer);
}

void buzzer_begin(buzzer_t* p_buzzer) {
    pinMode(p_buzzer->pin, OUTPUT);
}

void buzzer_upload(buzzer_t* p_buzzer) {
    if (!gnl_timer_check(p_buzzer->p_timer))
        return;

    if (p_buzzer->counter > 0)
        p_buzzer->counter--;

    if (p_buzzer->counter == 0) {
        p_buzzer->state = false;
        gnl_timer_stop(p_buzzer->p_timer);
    } else {
        p_buzzer->state = !p_buzzer->state;
    }

    if (p_buzzer->state)
        analogWrite(p_buzzer->pin, BU_ANALOG_VALUE);
    else
        analogWrite(p_buzzer->pin, 0);
}

void buzzer_beep(buzzer_t* p_buzzer, uint8_t quantity, bool slow) {
    gnl_timer_setup(p_buzzer->p_timer, slow ? BU_SLOW_BEEP_TIMER : BU_BEEP_TIMER, true);
    gnl_timer_start(p_buzzer->p_timer);
    p_buzzer->counter = quantity * 2;
    p_buzzer->state = false;
}