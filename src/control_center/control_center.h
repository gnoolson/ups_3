#ifndef CONTROL_CENTER_H
#define CONTROL_CENTER_H

#include <gnl_timer.h>
#include "../../config.h"
#include "../context/context.h"

typedef enum {
    CC_BS_CHARGING,
    CC_BS_CHARGING_ERROR,        // нема струму заряда
    CC_BS_DISCHARGING_CRITICAL,  // батарея майже розряджена
    CC_BS_DISCHARGING,
    CC_BS_STANDBY,
    CC_BS_UNKNOWN,
    CC_BS_EMPTY,
} battery_state_t;

typedef enum { CC_PS_GRID, CC_PS_INVERTER, CC_PS_NON } power_state_t;

typedef struct {
    context_t* p_context;
    gnl_timer_t* p_alarm_timer;
} control_center_t;

/*
 *
 *
 * */
control_center_t* cc_new_and_setup(context_t* p_context);
void cc_delete(control_center_t* p_cc);
void cc_update(control_center_t* p_cc);
battery_state_t cc_get_battery_state(control_center_t* p_cc);
power_state_t cc_get_power_state(control_center_t* p_cc);

#endif