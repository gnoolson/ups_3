#include "activity_manager.h"
#include "../display/display.h"
#include "../serial/serial.h"
#include "main/main_activity.h"
#include "settings/settings_activity.h"
#include "ui16s/ui16s_activity.h"
#include "ups/ups_settings_activity.h"
#include "start/start_activity.h"

static void actmgr_change_activity(activity_manager_t* p_actmgr, activity_id_t act_id) {
    activity_t* p_activity = p_actmgr->p_activity;
    if (p_activity != NULL) {
        p_activity->destroy(p_activity);
    }

    activity_t* p_new_activity = NULL;
    switch (act_id) {
		case ACT_ID_START:
            p_new_activity = (activity_t*)sta_new_and_setup(p_actmgr->p_context);
            break;
        case ACT_ID_MAIN:
            p_new_activity = (activity_t*)ma_new_and_setup(p_actmgr->p_context);
            break;
        case ACT_ID_SETTINGS:
            p_new_activity = (activity_t*)sa_new_and_setup(p_actmgr->p_context);
            break;
        case ACT_ID_SETTINGS_UPS_MODE:
			p_new_activity = (activity_t*)upssa_new_and_setup(p_actmgr->p_context);
            break;
        case ACT_ID_SETTINGS_GRID_RETURN_DELAY:
            p_new_activity = (activity_t*)ui16sa_new_and_setup(p_actmgr->p_context, UI16S_GRID_RETURN_DELAY);
            break;
        case ACT_ID_SETTINGS_START_INVERTER_DELAY:
            p_new_activity = (activity_t*)ui16sa_new_and_setup(p_actmgr->p_context, UI16S_START_INVERTER_DELAY);
            break;
        case ACT_ID_SETTINGS_ALARM_BATTERY_SOC:
            p_new_activity = (activity_t*)ui16sa_new_and_setup(p_actmgr->p_context, UI16S_ALARM_BATTERY_SOC);
            break;
        case ACT_ID_SETTINGS_CRITICAL_BATTERY_SOC:
            p_new_activity = (activity_t*)ui16sa_new_and_setup(p_actmgr->p_context, UI16S_CRITICAL_BATTERY_SOC);
            break;
        case ACT_ID_SETTINGS_START_CHARGING_SOC:
            p_new_activity = (activity_t*)ui16sa_new_and_setup(p_actmgr->p_context, UI16S_START_CHARGING_SOC);
            break;
        case ACT_ID_SETTINGS_STOP_CHARGING_SOC:
            p_new_activity = (activity_t*)ui16sa_new_and_setup(p_actmgr->p_context, UI16S_STOP_CHARGING_SOC);
            break;
        case ACT_ID_SETTINGS_SLEEP_TIMEOUT:
            p_new_activity = (activity_t*)ui16sa_new_and_setup(p_actmgr->p_context, UI16S_SLEEP_TIMEOUT);
            break;
    }

    p_actmgr->p_activity = p_new_activity;
}

/*
 *
 *
 * */
activity_manager_t* actmgr_new_and_setup(context_t* p_context) {
    activity_manager_t* p_actmgr = malloc(sizeof(activity_manager_t));
    p_actmgr->p_context = p_context;
    p_actmgr->p_activity = NULL;
    p_actmgr->p_timer = gnl_timer_new_and_setup(1000, true);

    return p_actmgr;
}

void actmgr_delete(activity_manager_t* p_actmgr) {
    if (p_actmgr->p_activity != NULL) {
        p_actmgr->p_activity->destroy(p_actmgr->p_activity);
    }

    gnl_timer_delete(p_actmgr->p_timer);
    free(p_actmgr);
}

void actmgr_begin(activity_manager_t* p_actmgr) {
    gnl_timer_start(p_actmgr->p_timer);
    actmgr_change_activity(p_actmgr, ACT_ID_START);
}

void actmgr_update(activity_manager_t* p_actmgr) {
    if (p_actmgr->p_activity == NULL)
        return;

    if (gnl_timer_check(p_actmgr->p_timer))
        p_actmgr->p_activity->heartbeat(p_actmgr->p_activity);

    p_actmgr->p_activity->update(p_actmgr->p_activity);

    activity_id_t next_action_id = activity_get_next_activity_id(p_actmgr->p_activity);
    if (next_action_id == ACT_ID_NON)
        return;

    actmgr_change_activity(p_actmgr, next_action_id);
}

void actmgr_process_buttons(activity_manager_t* p_actmgr, gnl_button_info_t** p_infos, uint8_t length) {
    if (p_actmgr->p_activity == NULL)
        return;

    display_t* p_display = context_get_object(p_actmgr->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);

    if (!display_is_light_on(p_display)) {
        display_light(p_display);
        return;
    }

    display_light(p_display);
    p_actmgr->p_activity->process_buttons(p_actmgr->p_activity, p_infos, length);
}
