#include "ui16s_activity.h"
#include "../../../config.h"
#include "../../display/display.h"
#include "../../serial/serial.h"
#include "../../settings/settings.h"

#define UI16S_ACTIVITY_GRID_RETURN_DELAY_MAX_VALUE 300  // sec
#define UI16S_ACTIVITY_GRID_RETURN_DELAY_MIN_VALUE 10   // sec

#define UI16S_ACTIVITY_START_INVERTER_DELAY_MAX_VALUE 300  // sec
#define UI16S_ACTIVITY_START_INVERTER_DELAY_MIN_VALUE 0    // sec

#ifdef DEV_MODE
#define UI16S_ACTIVITY_ALARM_BATTERY_SOC_MAX_VALUE 101  // %
#else
#define UI16S_ACTIVITY_ALARM_BATTERY_SOC_MAX_VALUE 20  // %
#endif
#define UI16S_ACTIVITY_ALARM_BATTERY_SOC_MIN_VALUE 4  // %

#ifdef DEV_MODE
#define UI16S_ACTIVITY_CRITICAL_BATTERY_SOC_MAX_VALUE 101 // %
#else
#define UI16S_ACTIVITY_CRITICAL_BATTERY_SOC_MAX_VALUE 10  // %
#endif
#define UI16S_ACTIVITY_CRITICAL_BATTERY_SOC_MIN_VALUE 2  // %

#ifdef DEV_MODE
#define UI16S_ACTIVITY_START_CHARGING_SOC_MAX_VALUE 101  // %
#else
#define UI16S_ACTIVITY_START_CHARGING_SOC_MAX_VALUE 98  // %
#endif
#define UI16S_ACTIVITY_START_CHARGING_SOC_MIN_VALUE 50  // %

#ifdef DEV_MODE
#define UI16S_ACTIVITY_STOP_CHARGING_SOC_MAX_VALUE 101  // %
#else
#define UI16S_ACTIVITY_STOP_CHARGING_SOC_MAX_VALUE 100  // %
#endif
#define UI16S_ACTIVITY_STOP_CHARGING_SOC_MIN_VALUE 60  // %

#define UI16S_ACTIVITY_SLEEP_TIMEOUT_MAX_VALUE 14400  // sec (4h)
#define UI16S_ACTIVITY_SLEEP_TIMEOUT_MIN_VALUE 60     // sec (1m)

static void print_first_row(ui16s_activity_t* p_ui16s) {
    activity_t* p_activity = (activity_t*)p_ui16s;
    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);

    if (p_ui16s->type == UI16S_GRID_RETURN_DELAY) {
        display_print_text(p_display, 0, 0, "Grid Return Delay");
    } else if (p_ui16s->type == UI16S_START_INVERTER_DELAY) {
        display_print_text(p_display, 0, 0, "Start Inverter Delay");
    } else if (p_ui16s->type == UI16S_ALARM_BATTERY_SOC) {
        display_print_text(p_display, 0, 0, "Alarm Battery SoC");
    } else if (p_ui16s->type == UI16S_CRITICAL_BATTERY_SOC) {
        display_print_text(p_display, 0, 0, "Critical Battery SoC");
    } else if (p_ui16s->type == UI16S_START_CHARGING_SOC) {
        display_print_text(p_display, 0, 0, "Start Charging SoC");
    } else if (p_ui16s->type == UI16S_STOP_CHARGING_SOC) {
        display_print_text(p_display, 0, 0, "Stop Charging SoC");
    } else if (p_ui16s->type == UI16S_SLEEP_TIMEOUT) {
        display_print_text(p_display, 0, 0, "Sleep Timeout");
    }
}

static void print_third_row(ui16s_activity_t* p_ui16s) {
    activity_t* p_activity = (activity_t*)p_ui16s;
    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_erase_after(p_display, 2, 0);

    if (p_ui16s->type == UI16S_GRID_RETURN_DELAY) {
        display_print_int(p_display, 2, 8, p_ui16s->value);

        if (p_ui16s->value < 100)
            display_print_text(p_display, 2, 11, "s");
        else
            display_print_text(p_display, 2, 12, "s");
    } else if (p_ui16s->type == UI16S_START_INVERTER_DELAY) {
        display_print_int(p_display, 2, 8, p_ui16s->value);

        if (p_ui16s->value < 10) {
            display_print_text(p_display, 2, 10, "s");
        } else if (p_ui16s->value < 100) {
            display_print_text(p_display, 2, 11, "s");
        } else {
            display_print_text(p_display, 2, 12, "s");
        }
    } else if (p_ui16s->type == UI16S_ALARM_BATTERY_SOC || p_ui16s->type == UI16S_CRITICAL_BATTERY_SOC || p_ui16s->type == UI16S_START_CHARGING_SOC ||
               p_ui16s->type == UI16S_STOP_CHARGING_SOC) {
        display_print_int(p_display, 2, 8, p_ui16s->value);

        if (p_ui16s->value < 10) {
            display_print_text(p_display, 2, 10, "%");
        } else if (p_ui16s->value < 100) {
            display_print_text(p_display, 2, 11, "%");
        } else {
            display_print_text(p_display, 2, 12, "%");
        }
    } else if (p_ui16s->type == UI16S_SLEEP_TIMEOUT) {
        int value = p_ui16s->value / 60;
        display_print_int(p_display, 2, 8, value);

        if (value < 10)
            display_print_text(p_display, 2, 10, "m");
        else
            display_print_text(p_display, 2, 11, "m");
    } else {
    }
}

static void add_value(ui16s_activity_t* p_ui16s, uint16_t value) {
    p_ui16s->value += value;

    if (p_ui16s->type == UI16S_GRID_RETURN_DELAY) {
        if (p_ui16s->value > UI16S_ACTIVITY_GRID_RETURN_DELAY_MAX_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_GRID_RETURN_DELAY_MAX_VALUE;
    } else if (p_ui16s->type == UI16S_START_INVERTER_DELAY) {
        if (p_ui16s->value > UI16S_ACTIVITY_START_INVERTER_DELAY_MAX_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_START_INVERTER_DELAY_MAX_VALUE;
    } else if (p_ui16s->type == UI16S_ALARM_BATTERY_SOC) {
        if (p_ui16s->value > UI16S_ACTIVITY_ALARM_BATTERY_SOC_MAX_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_ALARM_BATTERY_SOC_MAX_VALUE;
    } else if (p_ui16s->type == UI16S_CRITICAL_BATTERY_SOC) {
        if (p_ui16s->value > UI16S_ACTIVITY_CRITICAL_BATTERY_SOC_MAX_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_CRITICAL_BATTERY_SOC_MAX_VALUE;
    } else if (p_ui16s->type == UI16S_START_CHARGING_SOC) {
        if (p_ui16s->value > UI16S_ACTIVITY_START_CHARGING_SOC_MAX_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_START_CHARGING_SOC_MAX_VALUE;
    } else if (p_ui16s->type == UI16S_STOP_CHARGING_SOC) {
        if (p_ui16s->value > UI16S_ACTIVITY_STOP_CHARGING_SOC_MAX_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_STOP_CHARGING_SOC_MAX_VALUE;
    } else if (p_ui16s->type == UI16S_SLEEP_TIMEOUT) {
        if (p_ui16s->value > UI16S_ACTIVITY_SLEEP_TIMEOUT_MAX_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_SLEEP_TIMEOUT_MAX_VALUE;
    }
}

static void subtract_value(ui16s_activity_t* p_ui16s, uint16_t value) {
    p_ui16s->value -= value;

    if (p_ui16s->type == UI16S_GRID_RETURN_DELAY) {
        if (p_ui16s->value < UI16S_ACTIVITY_GRID_RETURN_DELAY_MIN_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_GRID_RETURN_DELAY_MIN_VALUE;
    } else if (p_ui16s->type == UI16S_START_INVERTER_DELAY) {
        if (p_ui16s->value < UI16S_ACTIVITY_START_INVERTER_DELAY_MIN_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_START_INVERTER_DELAY_MIN_VALUE;
    } else if (p_ui16s->type == UI16S_ALARM_BATTERY_SOC) {
        if (p_ui16s->value < UI16S_ACTIVITY_ALARM_BATTERY_SOC_MIN_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_ALARM_BATTERY_SOC_MIN_VALUE;
    } else if (p_ui16s->type == UI16S_CRITICAL_BATTERY_SOC) {
        if (p_ui16s->value < UI16S_ACTIVITY_CRITICAL_BATTERY_SOC_MIN_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_CRITICAL_BATTERY_SOC_MIN_VALUE;
    } else if (p_ui16s->type == UI16S_START_CHARGING_SOC) {
        if (p_ui16s->value < UI16S_ACTIVITY_START_CHARGING_SOC_MIN_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_START_CHARGING_SOC_MIN_VALUE;
    } else if (p_ui16s->type == UI16S_STOP_CHARGING_SOC) {
        if (p_ui16s->value < UI16S_ACTIVITY_STOP_CHARGING_SOC_MIN_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_STOP_CHARGING_SOC_MIN_VALUE;
    } else if (p_ui16s->type == UI16S_SLEEP_TIMEOUT) {
        if (p_ui16s->value < UI16S_ACTIVITY_SLEEP_TIMEOUT_MIN_VALUE)
            p_ui16s->value = UI16S_ACTIVITY_SLEEP_TIMEOUT_MIN_VALUE;
    }
}

static void save(ui16s_activity_t* p_ui16s) {
    activity_t* p_activity = (activity_t*)p_ui16s;
    settings_t* p_settings = (settings_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);

    if (p_ui16s->type == UI16S_GRID_RETURN_DELAY) {
        p_settings->grid_return_delay = p_ui16s->value;
    } else if (p_ui16s->type == UI16S_START_INVERTER_DELAY) {
        p_settings->start_inverter_delay = p_ui16s->value;
    } else if (p_ui16s->type == UI16S_ALARM_BATTERY_SOC) {
        p_settings->alarm_batt_soc = p_ui16s->value;
    } else if (p_ui16s->type == UI16S_CRITICAL_BATTERY_SOC) {
        p_settings->critical_batt_soc = p_ui16s->value;
    } else if (p_ui16s->type == UI16S_START_CHARGING_SOC) {
        p_settings->start_charging_soc = p_ui16s->value;
    } else if (p_ui16s->type == UI16S_STOP_CHARGING_SOC) {
        p_settings->stop_charging_soc = p_ui16s->value;
    } else if (p_ui16s->type == UI16S_SLEEP_TIMEOUT) {
        p_settings->sleep_timeout = p_ui16s->value;
    }

    settings_save(p_settings);
    p_ui16s->saved = true;
    gnl_timer_start(p_ui16s->p_save_value_timer);

    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_print_text(p_display, 3, 0, "ok");
}

/*
 *
 *
 * */
static void update(activity_t* p_activity) {
    ui16s_activity_t* p_ui16s = (ui16s_activity_t*)p_activity;
    if (p_ui16s->saved && gnl_timer_check(p_ui16s->p_save_value_timer)) {
        activity_change(p_activity, ACT_ID_SETTINGS);
    }
}

static void heartbeat(activity_t* p_activity) {}

static void destroy(activity_t* p_activity) {
    ui16sa_delete((ui16s_activity_t*)p_activity);
}

static void process_buttons(activity_t* p_activity, gnl_button_info_t** p_infos, uint8_t length) {
    ui16s_activity_t* p_ui16s = (ui16s_activity_t*)p_activity;
    if (p_ui16s->saved)
        return;

    uint16_t inc = 0;
    uint16_t fast_inc = 0;

    if (p_ui16s->type == UI16S_GRID_RETURN_DELAY || p_ui16s->type == UI16S_START_INVERTER_DELAY) {
        inc = 10;
        fast_inc = 50;
    } else if (p_ui16s->type == UI16S_SLEEP_TIMEOUT) {
        inc = 60;
        fast_inc = 300;
    } else if (p_ui16s->type == UI16S_ALARM_BATTERY_SOC || p_ui16s->type == UI16S_CRITICAL_BATTERY_SOC || p_ui16s->type == UI16S_START_CHARGING_SOC ||
               p_ui16s->type == UI16S_STOP_CHARGING_SOC) {
        inc = 1;
        fast_inc = 10;
    }

    for (int i = 0; i < length; i++) {
        int8_t button_id = p_infos[i]->button_id;
        gnl_button_state_t state = p_infos[i]->state;

        if (button_id == CONFIG_BUTTON_BACK_ID && state == GNL_BS_PRESSED) {
            activity_change(p_activity, ACT_ID_SETTINGS);
        } else if (button_id == CONFIG_BUTTON_DOWN_ID && state == GNL_BS_PRESSED) {
            subtract_value(p_ui16s, inc);
            print_third_row(p_ui16s);
        } else if (button_id == CONFIG_BUTTON_UP_ID && state == GNL_BS_PRESSED) {
            add_value(p_ui16s, inc);
            print_third_row(p_ui16s);
        } else if (button_id == CONFIG_BUTTON_ENTER_ID && state == GNL_BS_PRESSED) {
            save(p_ui16s);
        } else if (button_id == CONFIG_BUTTON_DOWN_ID && state == GNL_BS_LONG_PRESS && gnl_timer_check(p_ui16s->p_fast_change_value_timer)) {
            subtract_value(p_ui16s, fast_inc);
            print_third_row(p_ui16s);
        } else if (button_id == CONFIG_BUTTON_UP_ID && state == GNL_BS_LONG_PRESS && gnl_timer_check(p_ui16s->p_fast_change_value_timer)) {
            add_value(p_ui16s, fast_inc);
            print_third_row(p_ui16s);
        }
    }
}

/*
 *
 *
 * */
ui16s_activity_t* ui16sa_new_and_setup(context_t* p_context, ui16s_type_t type) {
    ui16s_activity_t* p_ui16s = (ui16s_activity_t*)malloc(sizeof(ui16s_activity_t));
    activity_setup(&(p_ui16s->base), p_context);

    p_ui16s->base.update = &update;
    p_ui16s->base.heartbeat = &heartbeat;
    p_ui16s->base.destroy = &destroy;
    p_ui16s->base.process_buttons = &process_buttons;
    p_ui16s->type = type;
    p_ui16s->saved = false;
    p_ui16s->p_fast_change_value_timer = gnl_timer_new_and_setup(1000, true);
    p_ui16s->p_save_value_timer = gnl_timer_new_and_setup(1500, false);

    gnl_timer_start(p_ui16s->p_fast_change_value_timer);

    settings_t* p_settings = (settings_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_SETTINGS);

    switch (type) {
        case (UI16S_GRID_RETURN_DELAY):
            p_ui16s->value = p_settings->grid_return_delay;
            break;
        case (UI16S_START_INVERTER_DELAY):
            p_ui16s->value = p_settings->start_inverter_delay;
            break;
        case (UI16S_ALARM_BATTERY_SOC):
            p_ui16s->value = p_settings->alarm_batt_soc;
            break;
        case (UI16S_CRITICAL_BATTERY_SOC):
            p_ui16s->value = p_settings->critical_batt_soc;
            break;
        case (UI16S_START_CHARGING_SOC):
            p_ui16s->value = p_settings->start_charging_soc;
            break;
        case (UI16S_STOP_CHARGING_SOC):
            p_ui16s->value = p_settings->stop_charging_soc;
            break;
        case (UI16S_SLEEP_TIMEOUT):
            p_ui16s->value = p_settings->sleep_timeout;
            break;
    }

    display_t* p_display = (display_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_clear(p_display);

    print_first_row(p_ui16s);
    print_third_row(p_ui16s);

    return p_ui16s;
}

void ui16sa_delete(ui16s_activity_t* p_ui16s) {
    gnl_timer_delete(p_ui16s->p_fast_change_value_timer);
    gnl_timer_delete(p_ui16s->p_save_value_timer);
    free(p_ui16s);
}
