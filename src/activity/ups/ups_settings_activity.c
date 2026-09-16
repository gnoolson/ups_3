#include "ups_settings_activity.h"
#include "../../../config.h"
#include "../../display/display.h"
#include "../../settings/settings.h"

static void print_first_row(ups_settings_activity_t* p_upssa) {
    activity_t* p_activity = (activity_t*)p_upssa;
    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_print_text(p_display, 0, 0, "UPS");
}

static void print_third_row(ups_settings_activity_t* p_upssa) {
    activity_t* p_activity = (activity_t*)p_upssa;
    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);

    display_erase_after(p_display, 2, 0);
    if (p_upssa->value) {
        display_print_text(p_display, 2, 9, "ON");
    } else {
        display_print_text(p_display, 2, 9, "OFF");
    }
}

static void change_value(ups_settings_activity_t* p_upssa) {
    p_upssa->value = !p_upssa->value;
}

static void save(ups_settings_activity_t* p_upssa) {
    activity_t* p_activity = (activity_t*)p_upssa;
    settings_t* p_settings = (settings_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_SETTINGS);

	p_settings->ups = p_upssa->value;

    settings_save(p_settings);
    p_upssa->saved = true;
    gnl_timer_start(p_upssa->p_save_value_timer);

    display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_print_text(p_display, 3, 0, "ok");
}

/*
 *
 *
 * */
static void update(activity_t* p_activity) {
    ups_settings_activity_t* p_upssa = (ups_settings_activity_t*)p_activity;
    if (p_upssa->saved && gnl_timer_check(p_upssa->p_save_value_timer)) {
        activity_change(p_activity, ACT_ID_SETTINGS);
    }
}

static void heartbeat(activity_t* p_activity) {}

static void destroy(activity_t* p_activity) {
    upssa_delete((ups_settings_activity_t*)p_activity);
}

static void process_buttons(activity_t* p_activity, gnl_button_info_t** p_infos, uint8_t length) {
    ups_settings_activity_t* p_upssa = (ups_settings_activity_t*)p_activity;
    if (p_upssa->saved)
        return;

    for (int i = 0; i < length; i++) {
        int8_t button_id = p_infos[i]->button_id;
        gnl_button_state_t state = p_infos[i]->state;

        if (button_id == CONFIG_BUTTON_BACK_ID && state == GNL_BS_PRESSED) {
            activity_change(p_activity, ACT_ID_SETTINGS);
        } else if (button_id == CONFIG_BUTTON_DOWN_ID && state == GNL_BS_PRESSED) {
            change_value(p_upssa);
            print_first_row(p_upssa);
            print_third_row(p_upssa);
        } else if (button_id == CONFIG_BUTTON_UP_ID && state == GNL_BS_PRESSED) {
            change_value(p_upssa);
            print_first_row(p_upssa);
            print_third_row(p_upssa);
        } else if (button_id == CONFIG_BUTTON_ENTER_ID && state == GNL_BS_PRESSED) {
			save(p_upssa);
        }
    }
}

/*
 *
 *
 * */
ups_settings_activity_t* upssa_new_and_setup(context_t* p_context) {
    ups_settings_activity_t* p_upssa = (ups_settings_activity_t*)malloc(sizeof(ups_settings_activity_t));
    activity_setup(&(p_upssa->base), p_context);

    p_upssa->base.update = &update;
    p_upssa->base.heartbeat = &heartbeat;
    p_upssa->base.destroy = &destroy;
    p_upssa->base.process_buttons = &process_buttons;
    p_upssa->saved = false;
    p_upssa->p_save_value_timer = gnl_timer_new_and_setup(1500, false);

    settings_t* p_settings = (settings_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_SETTINGS);
    p_upssa->value = p_settings->ups;

    display_t* p_display = (display_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_clear(p_display);

    print_first_row(p_upssa);
    print_third_row(p_upssa);

    return p_upssa;
}

void upssa_delete(ups_settings_activity_t* p_upssa) {
    gnl_timer_delete(p_upssa->p_save_value_timer);
    free(p_upssa);
}