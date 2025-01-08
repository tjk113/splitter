#pragma once

#include <stdint.h>

#include <fiesta/str.h>

#include "array.h"

typedef struct {
    str name;
    double time;
} Split;

Split split_create(str name);
void split_free(Split s);
_GENERATE_ARRAY_SIGNATURES(Split, split)

typedef struct {
    double start;
    double cur;
    bool running;
    bool finished;
} Timer;

void timer_start(Timer* t);
void timer_stop(Timer* t);
void timer_pause(Timer* t);
void timer_reset(Timer* t);
void timer_update(Timer* t);

typedef struct {
    Splits splits;
    int cur_split_index;
    double split_height;
    Timer timer;
    double timer_size;
} Layout;

void layout_draw(Layout l);