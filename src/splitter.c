#include <stdint.h>
#include <stdio.h>

#include <sys/time.h>

#include <fiesta/str.h>
#include <raylib.h>

#include "splitter.h"
#include "array.h"

Split split_create(str name) {
    return (Split){.name = name, .time = 0.0};
}

void split_free(Split s) {
    str_free(s.name);
}

_GENERATE_ARRAY_IMPLEMENTATIONS(Split, split)

// https://stackoverflow.com/a/31652223

// TODO: Un-jank this whole thing.

double time_centiseconds() {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((double)tv.tv_sec) + (tv.tv_usec / 10000);
}

double time_deciseconds() {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((double)tv.tv_sec) + (tv.tv_usec / 100000);
}

double time_seconds() {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return ((double)tv.tv_sec)
           + (tv.tv_usec / 1000000);
    // TODO: Draw font letters with consistent
    // spacing so that including more precision
    // doesn't cause the timer to jump around.
    //        + (time_deciseconds() / 10)
    //        + (time_centiseconds() / 100);
}

void timer_start(Timer* t) {
    t->start = time_seconds();
    t->cur = time_seconds();
    t->running = true;
    t->finished = false;
}

void timer_stop(Timer* t) {
    t->running = false;
    t->finished = true;
}

void timer_pause(Timer* t) {
    t->running = false;
}

void timer_reset(Timer* t) {
    t->start = 0.0;
    t->cur = 0.0;
    t->running = false;
    t->finished = false;
}

void timer_update(Timer* t) {
    t->cur = time_seconds();
}

// very hard-coded
void layout_draw(Layout l) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    // Draw splits
    Color split_color = DARKGRAY;
    int y_offset = 0;
    char text_buf[128] = {0};
    for (size_t i = 0; i < l.splits.len; ++i) {
        // Draw background
        DrawRectangle(0, y_offset, width, l.split_height, split_color);

        // Draw name
        sprintf(text_buf, "%s", splits_get(l.splits, i).name.data);
        DrawText(text_buf, 10, y_offset, l.split_height, WHITE);
        memset(text_buf, 0, sizeof(text_buf));

        // Draw time
        sprintf(text_buf, "0:%05.2f", splits_get(l.splits, i).time);
        DrawText(text_buf, width - MeasureText(text_buf, l.split_height), y_offset, l.split_height, WHITE);
        memset(text_buf, 0, sizeof(text_buf));

        y_offset += l.split_height;
        split_color = GRAY;
    }
    // Draw timer
    double delta = l.timer.cur - l.timer.start;
    sprintf(text_buf, "0:%05.2f", delta);
    // I'm not sure where the default value of 5.0 comes from for the spacing...
    Vector2 measurements = MeasureTextEx(GetFontDefault(), text_buf, l.timer_size, 5.0f);
    DrawText(text_buf, width - measurements.x, height - measurements.y, l.timer_size, WHITE);
}

int main() {
    // TODO: How to make a menu-less window?
    InitWindow(400, 800, "splitter");
    SetTargetFPS(60);

    Layout layout = (Layout){
        .splits = splits_create_from((Split[]){
            split_create(STR("One")),
            split_create(STR("Two")),
            (Split){0}
        }),
        .cur_split_index = -1,
        .split_height = 40,
        .timer = (Timer){.start = 0.0, .cur = 0.0, .running = false, .finished = false},
        .timer_size = 50
    };

    timer_start(&layout.timer);
    while (!WindowShouldClose()) {
        timer_update(&layout.timer);

        BeginDrawing();

        ClearBackground(BLACK);
        layout_draw(layout);

        EndDrawing();
    }
}