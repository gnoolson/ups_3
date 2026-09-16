#include "control_center.h"
#include "../bms/bms.h"
#include "../charger/charger.h"
#include "../grid/grid.h"
#include "../inverter/inverter.h"
#include "../load/load.h"
#include "../power_source_switch/power_source_switch.h"
#include "../serial/serial.h"
#include "../settings/settings.h"

/*
 *
 *
 * */
static bool is_inverter_selected(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    return pss_is_inverter_selected(p_pss);
}

static bool is_grid_ok(control_center_t* p_cc) {
    grid_t* p_grid = (grid_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_GRID);
    return grid_has_voltage(p_grid);
}

static bool is_grid_not_selected(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    return !pss_is_grid_selected(p_pss);
}

static bool is_grid_selected(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    return pss_is_grid_selected(p_pss);
}

static void select_grid_with_delay(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    pss_select_grid_with_delay(p_pss);
}

static void stop_select_grid(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    pss_stop_select_grid(p_pss);
}

static void select_grid(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    pss_select_grid_immediately(p_pss);
}

static bool does_it_make_sense_to_switch_power_source_to_inverter(control_center_t* p_cc) {
    settings_t* p_settings = (settings_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
    if (!p_settings->ups)
        return false;

    load_t* p_load = (load_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_LOAD);
    return load_exists(p_load);
}

static bool is_battery_ok(control_center_t* p_cc) {
    int8_t soc = bms_get_soc();
    if (soc < 0)
        return false;

    settings_t* p_settings = (settings_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
    return (int8_t)p_settings->critical_batt_soc < soc;
}

static bool is_inverter_not_selected(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    return !pss_is_inverter_selected(p_pss);
}

static void select_inverter_with_delay(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    pss_select_inverter_with_delay(p_pss);
}

static bool is_charger_ok(control_center_t* p_cc) {
    charger_t* p_charger = (charger_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_CHARGER);
    return charger_is_on(p_charger);
}

static bool does_it_make_sense_to_inverter_turn_on(control_center_t* p_cc) {
    settings_t* p_settings = (settings_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
    if (!p_settings->ups)
        return false;

    if (!is_battery_ok(p_cc))
        return false;

    load_t* p_load = (load_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_LOAD);
    return load_exists(p_load);
}

static void start_inverter(control_center_t* p_cc) {
    inverter_t* p_inverter = (inverter_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_INVERTER);
    inverter_turn_on(p_inverter);
}

static void stop_inverter(control_center_t* p_cc) {
    inverter_t* p_inverter = (inverter_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_INVERTER);
    inverter_turn_off(p_inverter);
}

static bool is_inverter_on(control_center_t* p_cc) {
    inverter_t* p_inverter = (inverter_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_INVERTER);
    return inverter_is_on(p_inverter);
}

static bool is_inverter_off(control_center_t* p_cc) {
    return !is_inverter_on(p_cc);
}

static bool is_charging_completed(control_center_t* p_cc) {
    int8_t soc = bms_get_soc();

    settings_t* p_settings = (settings_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
    if (p_settings->stop_charging_soc <= soc) {
        return true;
    }

    charger_t* p_charger = (charger_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_CHARGER);
    if (charger_is_on(p_charger) && !charger_is_preparing_to_turn_on(p_charger) && !charger_load_exists(p_charger)) {
        return true;
    }

    return false;
}

static void stop_charger(control_center_t* p_cc) {
    charger_t* p_charger = (charger_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_CHARGER);
    charger_turn_off(p_charger);
}

static void start_charger(control_center_t* p_cc) {
    charger_t* p_charger = (charger_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_CHARGER);
    charger_turn_on(p_charger);
}

static bool is_charging_required(control_center_t* p_cc) {
    int8_t soc = bms_get_soc();
    if (soc < 0)
        return false;

    settings_t* p_settings = (settings_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
    return p_settings->start_charging_soc >= soc;
}

static bool is_charger_preparing_to_start(control_center_t* p_cc) {
    charger_t* p_charger = (charger_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_CHARGER);
    return charger_is_preparing_to_turn_on(p_charger);
}

static bool is_charger_on(control_center_t* p_cc) {
    charger_t* p_charger = (charger_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_CHARGER);
    return charger_is_on(p_charger);
}

static void stop_select_inverter(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);
    pss_stop_select_inverter(p_pss);
}

static void stop_alarm_timer(control_center_t* p_cc) {
    gnl_timer_stop(p_cc->p_alarm_timer);
}

static bool is_battery_discharging(control_center_t* p_cc) {
    battery_state_t battery_state = cc_get_battery_state(p_cc);
    return battery_state == CC_BS_DISCHARGING || battery_state == CC_BS_DISCHARGING_CRITICAL;
}

static bool is_time_to_alarm(control_center_t* p_cc) {
    settings_t* p_settings = (settings_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
    return bms_get_soc() < p_settings->alarm_batt_soc;
}

static void alarm(control_center_t* p_cc) {
    bool beep = false;
    if (!p_cc->p_alarm_timer->active) {
        beep = true;
        gnl_timer_start(p_cc->p_alarm_timer);
    }

    if (gnl_timer_check(p_cc->p_alarm_timer))
        beep = true;

    if (beep) {
        buzzer_t* p_buzzer = (buzzer_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_BUZZER);
        buzzer_beep(p_buzzer, 5, false);
    }
}

/*
 *
 *
 * */
static void handle_inverter(control_center_t* p_cc) {
    if (is_inverter_selected(p_cc)) {
        if (does_it_make_sense_to_inverter_turn_on(p_cc)) {
            if (is_inverter_off(p_cc))
                start_inverter(p_cc);
        } else {
            if (is_inverter_on(p_cc))
                stop_inverter(p_cc);
        }
    } else {
        if (is_inverter_on(p_cc))
            stop_inverter(p_cc);
    }
}

static void handle_power_source_switch(control_center_t* p_cc) {
    if (!is_battery_ok(p_cc)) {
        stop_select_inverter(p_cc);
        select_grid(p_cc);
        return;
    }

    if (is_grid_ok(p_cc)) {
        stop_select_inverter(p_cc);

        if (is_grid_not_selected(p_cc)) {
            select_grid_with_delay(p_cc);
        }
    } else {
        stop_select_grid(p_cc);

        if (does_it_make_sense_to_switch_power_source_to_inverter(p_cc)) {
            if (is_battery_ok(p_cc)) {
                if (is_inverter_not_selected(p_cc)) {
                    select_inverter_with_delay(p_cc);
                }
            }
        } else {
            if (is_inverter_selected(p_cc)) {
                select_grid(p_cc);
            }
        }
    }
}

static void handle_battery(control_center_t* p_cc) {
    if (!is_battery_ok(p_cc) || !is_battery_discharging(p_cc) || !is_time_to_alarm(p_cc)) {
        stop_alarm_timer(p_cc);
        return;
    }

    alarm(p_cc);
}

static void handle_charger(control_center_t* p_cc) {
    if (is_grid_ok(p_cc) && is_grid_selected(p_cc)) {
        if (is_charger_on(p_cc)) {
            if (is_charging_completed(p_cc))
                stop_charger(p_cc);
        } else {
            if (is_charging_required(p_cc))
                start_charger(p_cc);
        }
    } else {
        if (is_charger_on(p_cc) || is_charger_preparing_to_start(p_cc))
            stop_charger(p_cc);
    }
}

/*
 *
 *
 * */
control_center_t* cc_new_and_setup(context_t* p_context) {
    control_center_t* p_cc = (control_center_t*)malloc(sizeof(control_center_t));
    p_cc->p_context = p_context;
    p_cc->p_alarm_timer = gnl_timer_new_and_setup(180000, true); // 5min

    return p_cc;
}

void cc_delete(control_center_t* p_cc) {
    gnl_timer_delete(p_cc->p_alarm_timer);
    free(p_cc);
}

void cc_update(control_center_t* p_cc) {
    handle_power_source_switch(p_cc);
    handle_charger(p_cc);
    handle_inverter(p_cc);
    handle_battery(p_cc);
}

battery_state_t cc_get_battery_state(control_center_t* p_cc) {
    context_t* p_context = p_cc->p_context;
    inverter_t* p_inverter = (inverter_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_INVERTER);

    if (inverter_is_ok(p_inverter) && is_inverter_selected(p_cc)) {
        if (is_time_to_alarm(p_cc)) {
            return CC_BS_DISCHARGING_CRITICAL;
        } else {
            return CC_BS_DISCHARGING;
        }
    }

    charger_t* p_charger = (charger_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_CHARGER);
    if (charger_is_on(p_charger)) {
        if (charger_is_ok(p_charger)) {
            return CC_BS_CHARGING;
        } else {
            return CC_BS_CHARGING_ERROR;
        }
    }

    uint8_t soc = bms_get_soc();

    if (soc == -1)
        return CC_BS_UNKNOWN;

    settings_t* p_settings = (settings_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
    if (p_settings->critical_batt_soc >= soc)
        return CC_BS_EMPTY;

    return CC_BS_STANDBY;
}

power_state_t cc_get_power_state(control_center_t* p_cc) {
    power_source_switch_t* p_pss = (power_source_switch_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH);

    if (pss_is_grid_selected(p_pss)) {
        if (is_grid_ok(p_cc)) {
            return CC_PS_GRID;
        } else {
            return CC_PS_NON;
        }
    } else {
        inverter_t* p_inverter = (inverter_t*)context_get_object(p_cc->p_context, CONFIG_CONTEXT_OBJ_INVERTER);
        if (inverter_is_ok(p_inverter)) {
            return CC_PS_INVERTER;
        } else {
            return CC_PS_NON;
        }
    }
}
