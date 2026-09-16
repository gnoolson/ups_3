#include "start_activity.h"
#include "../../../config.h"
#include "../../display/display.h"
#include "../../buzzer/buzzer.h"

static void print(start_activity_t* p_sa){
	activity_t* p_activity = (activity_t*)p_sa;
	display_t* p_display = (display_t*)context_get_object(p_activity->p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
	display_print_text(p_display, 1, 7, "UPS_3");
	display_print_text(p_display, 2, 7, "AnnaVi");
}

static void update(activity_t* p_activity) {
    start_activity_t* p_sa = (start_activity_t*)p_activity;
	
	if(gnl_timer_check(p_sa->p_timer))
		activity_change(p_activity, ACT_ID_MAIN);
}

static void heartbeat(activity_t* p_activity) {}

static void destroy(activity_t* p_activity) {
    sta_delete((start_activity_t*)p_activity);
}

static void process_buttons(activity_t* p_activity, gnl_button_info_t** p_infos, uint8_t length) {
    for (int i = 0; i < length; i++) {
        int8_t button_id = p_infos[i]->button_id;
        gnl_button_state_t state = p_infos[i]->state;

        if (button_id == CONFIG_BUTTON_ENTER_ID && state == GNL_BS_PRESSED) {
            activity_change(p_activity, ACT_ID_MAIN);
            return;
        }

        if (button_id == CONFIG_BUTTON_BACK_ID && state == GNL_BS_PRESSED) {
            activity_change(p_activity, ACT_ID_MAIN);
            return;
        }

        if (button_id == CONFIG_BUTTON_DOWN_ID && state == GNL_BS_PRESSED) {
            activity_change(p_activity, ACT_ID_MAIN);
            return;
        }

        if (button_id == CONFIG_BUTTON_UP_ID && state == GNL_BS_PRESSED) {
            activity_change(p_activity, ACT_ID_MAIN);
            return;
        }
    }
}
/*
 *
 *
 * */
start_activity_t* sta_new_and_setup(context_t* p_context) {
    start_activity_t* p_sa = (start_activity_t*)malloc(sizeof(start_activity_t));
    activity_setup((activity_t*)p_sa, p_context);
    p_sa->p_timer = gnl_timer_new_and_setup(10000, false);
    gnl_timer_start(p_sa->p_timer);

    p_sa->base.update = &update;
    p_sa->base.heartbeat = &heartbeat;
    p_sa->base.destroy = &destroy;
    p_sa->base.process_buttons = &process_buttons;

    display_t* p_display = (display_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_DISPLAY);
    display_clear(p_display);
	display_light(p_display);

    print(p_sa);
	
    buzzer_t* p_buzzer = (buzzer_t*)context_get_object(p_context, CONFIG_CONTEXT_OBJ_BUZZER);
	buzzer_beep(p_buzzer, 20, false);

    return p_sa;
}

void sta_delete(start_activity_t* p_sa) {
	gnl_timer_delete(p_sa->p_timer);
    free(p_sa);
}