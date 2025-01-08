#include <stdint.h>
#include <stdio.h>

#include <sys/time.h>

#include <fiesta/file.h>
#include <fiesta/str.h>
#include <raylib.h>

#include "splitter.h"
#include "array.h"

Split split_create(str name, double time) {
    return (Split){.name = name, .time = time};
}

void split_free(Split s) {
    str_free(s.name);
}

_GENERATE_ARRAY_IMPLEMENTATIONS(Split, split)

Splits splits_load(str filename) {
    File file = file_open(filename, FileRead);
    str_arr lines = file_read_lines(&file, 128);
    Splits splits = splits_create();
    for (size_t i = 0; i < lines.len; ++i) {
        str_arr parts = str_split(lines.data[i], ' ');
        str_println(parts.data[0]);
        splits_append(&splits, split_create(STR(parts.data[0].data), stod(parts.data[1])));
        str_arr_free(parts);
    }
    str_arr_free(lines);
    file_close(&file);
    return splits;
}

void splits_save(str filename, Splits splits) {
    File file = file_open(filename, FileWrite);
    for (size_t i = 0; i < splits.len; ++i) {
        char num_buf[128] = {0};
        sprintf(num_buf, "%f", splits.data[i].time);

        dynstr out = dynstr_create_from(splits.data[i].name.data);
        dynstr_append_char(&out, ' ');
        str num_str = str_create_from(num_buf);
        dynstr_append_str(&out, num_str);
        dynstr_append_char(&out, '\n');

        file_write_str(&file, dynstr_to_str(&out));
        str_free(num_str);
    }
    file_close(&file);
}

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

Timer timer_create() {
    return (Timer){
        .start = 0.0,
        .cur = 0.0,
        .running = false,
        .finished = false
    };
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

void timer_toggle_pause(Timer* t) {
    t->running = !t->running;
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

// TODO: These functions will control the timer
// as well as update visible layout elements,
// e.g. delta colors.

void splitter_start(SplitterState* ss) {
    timer_start(&ss->timer);
}

void splitter_stop(SplitterState* ss) {
    timer_stop(&ss->timer);
}

void splitter_toggle_pause(SplitterState* ss) {
    timer_toggle_pause(&ss->timer);
}

void splitter_update(SplitterState* ss) {
    timer_update(&ss->timer);
}

void splitter_split(SplitterState* ss) {
    if (ss->cur_split_index + 1 == ss->splits.len)
        timer_stop(&ss->timer);
    ss->splits.data[ss->cur_split_index++].time = ss->timer.cur - ss->timer.start;
}

void splitter_reset(SplitterState* ss) {
    timer_reset(&ss->timer);
    ss->cur_split_index = 0;
    // TODO: Load personal best splits instead
    // of resetting everything.
    for (size_t i = 0; i < ss->splits.len; ++i)
        ss->splits.data[i].time = 0.0;
}

// very hard-coded
void splitter_draw(SplitterState ss) {
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    // Draw splits
    Color split_color = DARKGRAY;
    int y_offset = 0;
    char text_buf[128] = {0};
    for (size_t i = 0; i < ss.splits.len; ++i) {
        // Draw background
        DrawRectangle(0, y_offset, width, ss.layout.split_height, split_color);

        // Draw name
        sprintf(text_buf, "%s", splits_get(ss.splits, i).name.data);
        DrawText(text_buf, 10, y_offset, ss.layout.split_height, WHITE);
        memset(text_buf, 0, sizeof(text_buf));

        // Draw time
        sprintf(text_buf, "0:%05.2f", splits_get(ss.splits, i).time);
        DrawText(text_buf, width - MeasureText(text_buf, ss.layout.split_height), y_offset, ss.layout.split_height, WHITE);
        memset(text_buf, 0, sizeof(text_buf));

        y_offset += ss.layout.split_height;
        split_color = GRAY;
    }
    // Draw timer
    double delta = ss.timer.cur - ss.timer.start;
    sprintf(text_buf, "0:%05.2f", delta);
    // I'm not sure where the default value of 5.0 comes from for the spacing...
    Vector2 measurements = MeasureTextEx(GetFontDefault(), text_buf, ss.layout.timer_size, 5.0f);
    DrawText(text_buf, width - measurements.x, height - measurements.y, ss.layout.timer_size, WHITE);
}

int main() {
    // TODO: How to make a menu-less window?
    InitWindow(400, 800, "splitter");
    SetTargetFPS(60);

    SplitterState ss = (SplitterState){
        .layout = (Layout){
            .split_height = 40,
            .timer_size = 50
        },
        .splits = splits_create_from((Split[]){
            split_create(STR("One"), 0.0),
            split_create(STR("Two"), 0.0),
            (Split){0}
        }),
        .cur_split_index = 0,
        .timer = timer_create(),
    };

    while (!WindowShouldClose()) {
        switch (GetKeyPressed()) {
            case KEY_SPACE: {
                if (ss.timer.finished)
                    splitter_reset(&ss);
                else if (!ss.timer.running)
                    splitter_start(&ss);
                else
                    splitter_split(&ss);
                break;
            }
            case KEY_P: {
                if (!ss.timer.finished)
                    splitter_toggle_pause(&ss);
                break;
            }
            case KEY_R: {
                splitter_reset(&ss);
                break;
            }
            // case KEY_S: {
            //     splits_save(STR("out.splits"), ss.splits);
            //     break;
            // }
            // case KEY_L: {
            //     if (ss.timer.running)
            //         splitter_reset(&ss);
            //     ss.splits = splits_load(STR("test.splits"));
            //     break;
            // }
        }

        if (ss.timer.running)
            splitter_update(&ss);

        BeginDrawing();

        ClearBackground(BLACK);
        splitter_draw(ss);

        EndDrawing();
    }
}