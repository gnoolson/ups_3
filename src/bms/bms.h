#ifndef BMS_H
#define BMS_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

int8_t bms_get_soc(void);
void bms_begin(void);
void bms_update(void);

#ifdef __cplusplus
}
#endif

#endif