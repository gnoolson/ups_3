#include "context.h"

context_t* context_new_and_setup(size_t length) {
    context_t* p_context = malloc(sizeof(context_t));
    if (p_context == NULL)
        return NULL;

    p_context->length = length;

    p_context->objects = malloc(sizeof(void*) * length);
    if (p_context->objects == NULL) {
        free(p_context);
        return NULL;
    }

    return p_context;
}

void context_delete(context_t* p_context) {
    free(p_context->objects);
    free(p_context);
}

bool context_set_object(context_t* p_context, uint8_t index, void* object) {
    if (index >= p_context->length)
        return false;

    p_context->objects[index] = object;
    return true;
}

void* context_get_object(context_t* p_context, uint8_t index) {
    if (index >= p_context->length) 
        return NULL;
    
    return p_context->objects[index];
}