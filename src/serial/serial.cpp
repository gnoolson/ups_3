#include "serial.h"
#include <Arduino.h>

extern "C" {

void serial_print_text(const char* text) {
    Serial.print(text);
}

void serial_print_char(char value) {
    Serial.print(value);
}

void serial_print_long(long value) {
    Serial.print(value);
}

void serial_print_int(int value) {
    Serial.print(value);
}

void serial_print_float(float value) {
    Serial.print(value);
}

void serial_println_text(const char* text) {
    Serial.println(text);
}

void serial_println_char(char value) {
    Serial.println(value);
}

void serial_println_long(long value) {
    Serial.println(value);
}

void serial_println_int(int value) {
    Serial.println(value);
}

void serial_println_float(float value) {
    Serial.println(value);
}
}