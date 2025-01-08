#pragma once

#include <stdlib.h>
#include <string.h>

typedef struct {
    void* data;
    int len;
    int cap;
} dynobj;

#define DYN_BASE_SIZE     10
#define DYN_GROWTH_RATE 1.5f

void maybe_realloc(dynobj* obj, int num_new_elements, int element_size);

/* It's expected that types used with these macros have a free function with the
signature `void <prefix>_free(<type> x)`. For example, `void str_free(str s)`. */

#define _GENERATE_ARRAY_SIGNATURES(type, prefix)   \
    typedef struct {                               \
        type* data;                                \
        int len;                                   \
        int cap;                                   \
    } type##s;                                     \
    type##s prefix##s_create(void);                \
    type##s prefix##s_create_from(type* arr);      \
    void prefix##s_free(type##s arr);              \
    type prefix##s_get(type##s arr, int index);    \
    void prefix##s_append(type##s* arr, type new); \
    type prefix##s_remove(type##s* arr, int index);

#define _GENERATE_ARRAY_IMPLEMENTATIONS(type, prefix)               \
    type##s prefix##s_create(void) {                                \
        type##s new;                                                \
                                                                    \
        new.cap = DYN_BASE_SIZE;                                    \
        new.len = 0;                                                \
        new.data = malloc(sizeof(type) * new.cap);                  \
                                                                    \
        return new;                                                 \
    }                                                               \
                                                                    \
    type##s prefix##s_create_from(type* arr) {                      \
        type##s new = {0};                                          \
                                                                    \
                                                                    \
        size_t arr_len = 0;                                         \
        while (memcmp(&arr[arr_len], &(type){0}, sizeof(type)))     \
            ++arr_len;                                              \
                                                                    \
        new.cap = arr_len * DYN_GROWTH_RATE;                        \
        new.len = arr_len;                                          \
        new.data = malloc(sizeof(type) * new.cap);                  \
        memcpy(new.data, arr, sizeof(type) * arr_len);              \
                                                                    \
        return new;                                                 \
    }                                                               \
                                                                    \
    void prefix##s_free(type##s arr) {                              \
        for (int i = 0; i < arr.len; i++)                           \
            prefix##_free(arr.data[i]);                             \
        free(arr.data);                                             \
    }                                                               \
                                                                    \
    type prefix##s_get(type##s arr, int index) {                    \
        if (index >= arr.len || index < 0)                          \
            return (type){0};                                       \
        return arr.data[index];                                     \
    }                                                               \
                                                                    \
    void prefix##s_append(type##s* arr, type new) {                 \
        maybe_realloc((dynobj*)arr, 1, sizeof(type));               \
                                                                    \
        memcpy(&arr->data[arr->len], &new, sizeof(type));           \
        arr->len++;                                                 \
    }                                                               \
                                                                    \
    type prefix##s_remove(type##s* arr, int index) {                \
        if (index < 0 || index >= arr->len)                         \
            return (type){0};                                       \
        type removed;                                               \
        memcpy(&removed, &arr->data[index], sizeof(type));          \
        arr->data[index] = (type){0};                               \
        for (int i = index; i < arr->len; i++) {                    \
            arr->data[i] = arr->data[i+1];                          \
        }                                                           \
        arr->len--;                                                 \
        maybe_realloc((dynobj*)arr, -1, sizeof(type));              \
                                                                    \
        return removed;                                             \
    }
