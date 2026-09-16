#include "load.h"
#include "../serial/serial.h"

load_t* load_new_and_setup(settings_t* p_settings, uint8_t sensor_pin) {
    load_t* p_load = malloc(sizeof(load_t));
    p_load->p_ds = ds_new_and_setup(sensor_pin);
    p_load->p_settings = p_settings;
    p_load->flag = false;
    p_load->inactivity_time = p_settings->sleep_timeout;
    p_load->p_timer = gnl_timer_new_and_setup(1000, true);
    gnl_timer_start(p_load->p_timer);

    return p_load;
}

void load_delete(load_t* p_load) {
    ds_delete(p_load->p_ds);
	gnl_timer_delete(p_load->p_timer);
    free(p_load);
}

void load_begin(load_t* p_load) {
    ds_begin(p_load->p_ds);
}

void load_update(load_t* p_load) {
    if (!gnl_timer_check(p_load->p_timer))
        return;
 
    ds_update(p_load->p_ds);
    bool real_state = ds_get_state(p_load->p_ds);

    if (real_state) {
        p_load->inactivity_time = 0;
        p_load->flag = true;
    } else {
		if(p_load->flag) {
			p_load->inactivity_time++;
			if (p_load->inactivity_time > p_load->p_settings->sleep_timeout) {
				p_load->inactivity_time = p_load->p_settings->sleep_timeout;
			}
			p_load->flag = p_load->inactivity_time != p_load->p_settings->sleep_timeout;
		} else {
			p_load->inactivity_time = p_load->p_settings->sleep_timeout;
		}
    }
}

bool load_exists(load_t* p_load) {
    return p_load->flag;
}

bool load_is_existing_right_now(load_t* p_load){
	return ds_get_state(p_load->p_ds);
}

void load_control(load_t* p_load, bool flag){
	if(flag) {
		p_load->inactivity_time = 0;
        p_load->flag = true;
	} else {
		p_load->flag = false;
		p_load->inactivity_time = p_load->p_settings->sleep_timeout;
	}
}
