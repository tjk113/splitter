#include <stdlib.h>

#include "array.h"

void maybe_realloc(dynobj* obj, int num_new_elements, int element_size) {
    // Allocate more memory if needed.
    if (obj->len + num_new_elements >= obj->cap) {
        obj->cap += num_new_elements;
        obj->cap *= DYN_GROWTH_RATE;
        void* new_ptr = realloc(obj->data, obj->cap * element_size);
        if (new_ptr) obj->data = new_ptr;
        memset(new_ptr + obj->len + num_new_elements, 0, num_new_elements * element_size);
    }
    // Reduce memory if over-allocated.
    else if (num_new_elements < 0 && num_new_elements != -obj->len) {
        obj->cap -= num_new_elements;
        void* new_ptr = realloc(obj->data, obj->cap * element_size);
        if (new_ptr) obj->data = new_ptr;
        memset(new_ptr + obj->len + num_new_elements, 0, num_new_elements * element_size);
    }
    /* But don't free the dynobj if clearing all of its elements
    (passing 0 to realloc would free the allocation). */
    else if (num_new_elements == -obj->len) {
        obj->cap = DYN_BASE_SIZE;
        void* new_ptr = realloc(obj->data, obj->cap * element_size);
        if (new_ptr) obj->data = new_ptr;
        memset(new_ptr, 0, obj->cap * element_size);
    }
}