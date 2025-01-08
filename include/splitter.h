#pragma once

#include <stdint.h>

#include <fiesta/str.h>

#include "array.h"

typedef struct {
    str name;
    double time;
} Split;

Split split_create(str name, double time);
void split_free(Split s);
_GENERATE_ARRAY_SIGNATURES(Split, split)

Splits splits_load(str filename);
void splits_save(str filename, Splits splits);

typedef struct {
    double start;
    double cur;
    bool running;
    bool finished;
} Timer;

Timer timer_create();
void timer_start(Timer* t);
void timer_stop(Timer* t);
void timer_toggle_pause(Timer* t);
void timer_reset(Timer* t);
void timer_update(Timer* t);

typedef struct {
    double split_height;
    double timer_size;
} Layout;

typedef struct {
    Layout layout;
    Splits splits;
    int cur_split_index;
    Timer timer;
} SplitterState;

void splitter_start(SplitterState* ss);
void splitter_stop(SplitterState* ss);
void splitter_toggle_pause(SplitterState* ss);
void splitter_reset(SplitterState* ss);
void splitter_update(SplitterState* ss);
void splitter_split(SplitterState* ss);
void splitter_load_run(SplitterState* ss, Splits s);
void splitter_save_run(SplitterState* ss, Splits s);
void splitter_draw(SplitterState ss);