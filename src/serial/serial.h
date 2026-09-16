#ifndef SERIAL_H
#define SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

void serial_print_text(const char* text);
void serial_print_char(char value);
void serial_print_long(long value);
void serial_print_int(int value);
void serial_print_float(float value);

void serial_println_text(const char* text);
void serial_println_char(char value);
void serial_println_long(long value);
void serial_println_int(int value);
void serial_println_float(float value);

#ifdef __cplusplus
}
#endif

#endif