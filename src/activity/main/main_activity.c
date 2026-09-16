#include "main_activity.h"
#include "../../../config.h"
#include "../../bms/bms.h"
#include "../../control_center/control_center.h"
#include "../../display/display.h"
#include "../../grid/grid.h"
#include "../../inverter/inverter.h"
#include "../../load/load.h"
#include "../../serial/serial.h"
#include "../../settings/settings.h"

static void print_first_row(main_activity_t* p_ma) {
    activity_t* p_activity = (activity_t*)p_ma;
    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    control_center_t* p_cc = (control_center_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_CONTROL_CENTER);
    battery_state_t bs = cc_get_battery_state(p_cc);

    display_print_text(p_display, 0, 0, "Batt: ");
    const int8_t soc = bms_get_soc();
    const uint8_t row = 0;

    if (bs != CC_BS_UNKNOWN) {
        display_print_int(p_display, row, 6, soc);
        if (soc < 10)
            display_print_text(p_display, row, 7, "%  ");
        else if (soc < 100)
            display_print_text(p_display, row, 8, "% ");
        else
            display_print_text(p_display, row, 9, "%");
    } else {
        display_print_text(p_display, row, 6, "    ");
    }

    switch (bs) {
        case CC_BS_EMPTY:
            display_print_text(p_display, row, 15, "  EMP");
            break;
        case CC_BS_CHARGING:
            display_print_text(p_display, row, 15, "  CHG");
            break;
        case CC_BS_CHARGING_ERROR:
            display_print_text(p_display, row, 15, " !CHG");
            break;
        case CC_BS_DISCHARGING:
            display_print_text(p_display, row, 15, " DCHG");
            break;
        case CC_BS_DISCHARGING_CRITICAL:
            display_print_text(p_display, row, 15, "!DCHG");
            break;
        case CC_BS_STANDBY:
            display_print_text(p_display, row, 15, "     ");
            break;
        case CC_BS_UNKNOWN:
            display_print_text(p_display, row, 15, "  UNK");
            break;
    }
}

static void print_second_row(main_activity_t* p_ma) {
    activity_t* p_activity = (activity_t*)p_ma;
    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    control_center_t* p_cc = (control_center_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_CONTROL_CENTER);

    power_state_t ps = cc_get_power_state(p_cc);
    const uint8_t row = 1;
    display_print_text(p_display, row, 0, "Output: ");

    switch (ps) {
        case CC_PS_GRID: {
            grid_t* p_grid = (grid_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_GRID);

            display_print_text(p_display, row, 8, "GRID        ");
        } break;
        case CC_PS_INVERTER: {
            display_print_text(p_display, row, 8, "INV         ");
        } break;
        case CC_PS_NON:
            display_print_text(p_display, row, 8, "-           ");
            break;
    }
}

static void print_third_row(main_activity_t* p_ma) {
    activity_t* p_activity = (activity_t*)p_ma;
    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    grid_t* p_grid = (grid_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_GRID);

    const uint8_t row = 2;

    display_print_text(p_display, row, 0, "Grid: ");
    if (grid_has_voltage(p_grid)) {
        display_print_text(p_display, row, 6, "+");
    } else {
        display_print_text(p_display, row, 6, "-");
    }
}

static void print_fourth_row(main_activity_t* p_ma) {
    activity_t* p_activity = (activity_t*)p_ma;
    control_center_t* p_cc = (control_center_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_CONTROL_CENTER);
    power_state_t ps = cc_get_power_state(p_cc);
    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    const uint8_t row = 3;

    load_t* p_load = (load_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_LOAD);

    if (load_exists(p_load) && load_is_existing_right_now(p_load)) {
        display_print_text(p_display, row, 0, "Load: +             ");
    } else if (load_exists(p_load) && !load_is_existing_right_now(p_load)) {
        display_print_text(p_display, row, 0, "Load: -     ");

        settings_t* p_settings = (settings_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
        const uint16_t value = p_settings->sleep_timeout - p_load->inactivity_time;
        const uint8_t hours = value / 3600;
        const uint8_t minutes = (value % 3600) / 60;
        const uint8_t seconds = value % 60;
        const uint8_t col = 12;

        if (hours < 10) {
            display_print_int(p_display, row, col, 0);
            display_print_int(p_display, row, col + 1, hours);
        } else {
            display_print_int(p_display, row, col, hours);
        }

        display_print_text(p_display, row, col + 2, ":");

        if (minutes < 10) {
            display_print_int(p_display, row, col + 3, 0);
            display_print_int(p_display, row, col + 4, minutes);
        } else {
            display_print_int(p_display, row, col + 3, minutes);
        }

        display_print_text(p_display, row, col + 5, ":");

        if (seconds < 10) {
            display_print_int(p_display, row, col + 6, 0);
            display_print_int(p_display, row, col + 7, seconds);
        } else {
            display_print_int(p_display, row, col + 6, seconds);
        }

        display_erase_after(p_display, row, col + 8);
    } else if (!load_exists(p_load)) {
        display_print_text(p_display, row, 0, "Load: -             ");
    }
}

static void update(activity_t* p_activity) {
    //
}

static void heartbeat(activity_t* p_activity) {
    main_activity_t* p_ma = (main_activity_t*)p_activity;
    print_first_row(p_ma);
    print_second_row(p_ma);
    print_third_row(p_ma);
    print_fourth_row(p_ma);
}

static void destroy(activity_t* p_activity) {
    ma_delete((main_activity_t*)p_activity);
}

static void process_buttons(activity_t* p_activity, gnl_button_info_t** p_infos, uint8_t length) {
    for (int i = 0; i < length; i++) {
        const int8_t button_id = p_infos[i]->button_id;
        gnl_button_state_t state = p_infos[i]->state;

        if (button_id == CONFIG_BUTTON_ENTER_ID && state == GNL_BS_PRESSED) {
            activity_change(p_activity, ACT_ID_SETTINGS);
            return;
        }

        if (button_id == CONFIG_BUTTON_DOWN_ID && state == GNL_BS_LONG_PRESS) {
            load_t* p_load = (load_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_LOAD);
            load_control(p_load, false);
            return;
        }

        if (button_id == CONFIG_BUTTON_UP_ID && state == GNL_BS_LONG_PRESS) {
            load_t* p_load = (load_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_LOAD);
            load_control(p_load, true);
            return;
        }
    }
}

/*
 *
 *
 * */
main_activity_t* ma_new_and_setup(context_t* p_context) {
    main_activity_t* p_ma = (main_activity_t*)malloc(sizeof(main_activity_t));
    activity_setup((activity_t*)p_ma, p_context);
    p_ma->base.update = &update;
    p_ma->base.heartbeat = &heartbeat;
    p_ma->base.destroy = &destroy;
    p_ma->base.process_buttons = &process_buttons;

    display_t* p_display = (display_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_clear(p_display);

    print_first_row(p_ma);
    print_second_row(p_ma);
    print_third_row(p_ma);
    print_fourth_row(p_ma);

    return p_ma;
}

void ma_delete(main_activity_t* p_ma) {
    free(p_ma);
}