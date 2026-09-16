#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <gnl_timer.h>

typedef struct {
    gnl_timer_t* p_timer;
	void* p_lcd;
} display_t;

/*
 *
 *
 * */
display_t* display_new_and_setup(byte address);
void display_delete(display_t* p_display);
void display_begin(display_t* p_display);
void display_erase_after(display_t* p_display, uint8_t row, uint8_t position);
void display_print_text(display_t* p_display, uint8_t row, uint8_t position, const char *text);
void display_print_int(display_t* p_display, uint8_t row, uint8_t position, int number);
void display_clear(display_t* p_display);
void display_light(display_t* p_display);
void display_update(display_t* p_display);
bool display_is_light_on(display_t* p_display);

#ifdef __cplusplus
}
#endif

#endif