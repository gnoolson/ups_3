#ifndef CONTEXT_H
#define CONTEXT_H

#include <Arduino.h>

typedef struct context {
    void** objects;
    uint8_t length;
} context_t;

context_t* context_new_and_setup(size_t length);
void context_delete(context_t* p_context);
bool context_set_object(context_t* p_context, uint8_t index, void* object);
void* context_get_object(context_t* p_context, uint8_t index);

#endif
