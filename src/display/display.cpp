#include "display.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ROWS 4
#define LCD_SYMBOLS 20
#define DISPLAY_CLEAN_STRING "                    "

static void set_cursor(display_t* p_display, uint8_t row, uint8_t position) {
    LiquidCrystal_I2C* p_lcd = (LiquidCrystal_I2C*)p_display->p_lcd;
    p_lcd->setCursor(position, row);
}

static void print_text(display_t* p_display,  const char* text) {
    LiquidCrystal_I2C* p_lcd = (LiquidCrystal_I2C*)p_display->p_lcd;
    p_lcd->print(text);
}

static void print_number(display_t* p_display, int number) {
    LiquidCrystal_I2C* p_lcd = (LiquidCrystal_I2C*)p_display->p_lcd;
    p_lcd->print(number);
}

/*
 *
 *
 * */
display_t* display_new_and_setup(byte address) {
    display_t* p_display = (display_t*)malloc(sizeof(display_t));
    p_display->p_timer = gnl_timer_new_and_setup(60000, false);
    p_display->p_lcd = new LiquidCrystal_I2C(address, LCD_SYMBOLS, LCD_ROWS);

    return p_display;
}

void display_delete(display_t* p_display) {
    delete (LiquidCrystal_I2C*)p_display->p_lcd;
    gnl_timer_delete(p_display->p_timer);
    free(p_display);
}

void display_begin(display_t* p_display) {
    LiquidCrystal_I2C* p_lcd = (LiquidCrystal_I2C*)p_display->p_lcd;
    p_lcd->init();
    p_lcd->clear();
}

void display_erase_after(display_t* p_display, uint8_t row, uint8_t position) {
    set_cursor(p_display, row, position);
    print_text(p_display, &DISPLAY_CLEAN_STRING[position]);
}

void display_print_text(display_t* p_display, uint8_t row, uint8_t position, const char* text) {
    set_cursor(p_display, row, position);
    print_text(p_display, text);
}

void display_print_int(display_t* p_display, uint8_t row, uint8_t position, int number) {
    set_cursor(p_display, row, position);
    print_number(p_display, number);
}

void display_clear(display_t* p_display) {
    LiquidCrystal_I2C* p_lcd = (LiquidCrystal_I2C*)p_display->p_lcd;
    p_lcd->clear();
}

void display_light(display_t* p_display) {
    LiquidCrystal_I2C* p_lcd = (LiquidCrystal_I2C*)p_display->p_lcd;
    p_lcd->backlight();
    gnl_timer_start(p_display->p_timer);
}

void display_update(display_t* p_display) {
    if (!gnl_timer_check(p_display->p_timer))
        return;

    LiquidCrystal_I2C* p_lcd = (LiquidCrystal_I2C*)p_display->p_lcd;
    p_lcd->noBacklight();
}

bool display_is_light_on(display_t* p_display) {
    return p_display->p_timer->active;
}