#include "activity.h"

void activity_setup(activity_t* p_activity, context_t* p_context){
	p_activity->next_activity = ACT_ID_NON;
	p_activity->p_context = p_context;
}

void activity_change(activity_t* p_activity, activity_id_t activity_id) {
	p_activity->next_activity = activity_id;
}

activity_id_t activity_get_next_activity_id(activity_t* p_activity) {
	return p_activity->next_activity;
}