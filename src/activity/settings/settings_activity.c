#include "settings_activity.h"
#include "../../../config.h"
#include "../../display/display.h"
#include "../../serial/serial.h"

#define SETTINGS_ACTIVITY_RETURN_TO_MAIN_MENU_DELAY 60
#define SETTINGS_ACTIVITY_MENU_LIST_SIZE 8

static const char* menu_list[] = {
    "UPS", "Grid Return Dly", "Inverter Start Dly", "Alarm Batt SoC", "Critical Batt SoC", "Start Charging SoC", "Stop Charging SoC", "Sleep Timeout"};

/*
 *
 *
 * */
static void print_row(settings_activity_t* p_sa, uint8_t row) {
    display_t* p_display = (display_t*)context_get_object(((activity_t*)p_sa)->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    if (p_sa->row_cursor == row) {
        display_print_text(p_display, row, 0, "#");
        p_sa->selected_row = p_sa->first_row_of_list + row;
    } else {
        display_print_text(p_display, row, 0, " ");
    }

    const char* str = menu_list[p_sa->first_row_of_list + row];
    display_print_text(p_display, row, 1, str);
    display_erase_after(p_display, row, strlen(str) + 1);
}

static void cursor_down(settings_activity_t* p_sa) {
    p_sa->row_cursor++;
    if (p_sa->row_cursor > 3) {
        p_sa->row_cursor = 3;
        p_sa->first_row_of_list++;
        if (p_sa->first_row_of_list == SETTINGS_ACTIVITY_MENU_LIST_SIZE - 3) {
            p_sa->first_row_of_list = SETTINGS_ACTIVITY_MENU_LIST_SIZE - 4;
            return;
        }
    }

    print_row(p_sa, 0);
    print_row(p_sa, 1);
    print_row(p_sa, 2);
    print_row(p_sa, 3);
}

static void cursor_up(settings_activity_t* p_sa) {
    p_sa->row_cursor--;
    if (p_sa->row_cursor < 0) {
        p_sa->row_cursor = 0;
        p_sa->first_row_of_list--;
        if (p_sa->first_row_of_list < 0) {
            p_sa->first_row_of_list = 0;
            return;
        }
    }

    print_row(p_sa, 0);
    print_row(p_sa, 1);
    print_row(p_sa, 2);
    print_row(p_sa, 3);
}

static void select(settings_activity_t* p_sa) {
    activity_t* p_activity = (activity_t*)p_sa;
    switch (p_sa->selected_row) {
        case 0:
            activity_change(p_activity, ACT_ID_SETTINGS_UPS_MODE);
            break;

        case 1:
            activity_change(p_activity, ACT_ID_SETTINGS_GRID_RETURN_DELAY);
            break;

        case 2:
            activity_change(p_activity, ACT_ID_SETTINGS_START_INVERTER_DELAY);
            break;

        case 3:
            activity_change(p_activity, ACT_ID_SETTINGS_ALARM_BATTERY_SOC);
            break;

        case 4:
            activity_change(p_activity, ACT_ID_SETTINGS_CRITICAL_BATTERY_SOC);
            break;

        case 5:
            activity_change(p_activity, ACT_ID_SETTINGS_START_CHARGING_SOC);
            break;

        case 6:
            activity_change(p_activity, ACT_ID_SETTINGS_STOP_CHARGING_SOC);
            break;

        case 7:
            activity_change(p_activity, ACT_ID_SETTINGS_SLEEP_TIMEOUT);
            break;
    }
}

/*
 *
 *
 * */
static void update(activity_t* p_activity) {}

static void heartbeat(activity_t* p_activity) {}

static void destroy(activity_t* p_activity) {
    settings_activity_t* p_sa = (settings_activity_t*)p_activity;
    sa_delete(p_sa);
}

static void process_buttons(activity_t* p_activity, gnl_button_info_t** p_infos, uint8_t length) {
    settings_activity_t* p_sa = (settings_activity_t*)p_activity;

    for (int i = 0; i < length; i++) {
        int8_t button_id = p_infos[i]->button_id;
        gnl_button_state_t state = p_infos[i]->state;

        if (button_id == CONFIG_BUTTON_BACK_ID && state == GNL_BS_PRESSED) {
            activity_change(p_activity, ACT_ID_MAIN);
        } else if (button_id == CONFIG_BUTTON_DOWN_ID && state == GNL_BS_PRESSED) {
            cursor_down(p_sa);
        } else if (button_id == CONFIG_BUTTON_UP_ID && state == GNL_BS_PRESSED) {
            cursor_up(p_sa);
        } else if (button_id == CONFIG_BUTTON_ENTER_ID && state == GNL_BS_PRESSED) {
            select(p_sa);
        }
    }
}

/*
 *
 *
 * */
settings_activity_t* sa_new_and_setup(context_t* p_context) {
    settings_activity_t* p_sa = (settings_activity_t*)malloc(sizeof(settings_activity_t));
    activity_setup(&(p_sa->base), p_context);

    p_sa->base.update = &update;
    p_sa->base.heartbeat = &heartbeat;
    p_sa->base.destroy = &destroy;
    p_sa->base.process_buttons = &process_buttons;
    p_sa->row_cursor = 0;
    p_sa->first_row_of_list = 0;
    p_sa->selected_row = 0;

    display_t* p_display = (display_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_clear(p_display);
    print_row(p_sa, 0);
    print_row(p_sa, 1);
    print_row(p_sa, 2);
    print_row(p_sa, 3);

    return p_sa;
}

void sa_delete(settings_activity_t* p_sa) {
    free(p_sa);
}