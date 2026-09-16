#ifndef MAIN_ACTIVITY_H
#define MAIN_ACTIVITY_H

#include "../activity.h"

typedef struct {
	activity_t base;
	//
} main_activity_t;

/*
 * 
 * 
 * */
main_activity_t* ma_new_and_setup(context_t* p_context);
void ma_delete(main_activity_t* p_ma);

#endif