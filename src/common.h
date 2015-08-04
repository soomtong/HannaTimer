//
// Created by Rogue Knight on 2015. 8. 2..
//
#include <pebble.h>

#ifndef HANNATIMER_COMMON_H
#define HANNATIMER_COMMON_H
#endif //HANNATIMER_COMMON_H

#define LAP_TIMER_SIZE 32
#define STOP_TIMER_SIZE 5

#define PERSIST_KEY_ID_MODE 1
#define PERSIST_KEY_ID_LAPS 2
#define PERSIST_KEY_ID_LAP_COUNTER 3
#define PERSIST_KEY_ID_STOP_PICK_COUNTER 4
#define PERSIST_KEY_ID_STOP_TIMER_0 5
#define PERSIST_KEY_ID_STOP_TIMER_1 6
#define PERSIST_KEY_ID_STOP_TIMER_2 7
#define PERSIST_KEY_ID_STOP_TIMER_3 8
#define PERSIST_KEY_ID_STOP_TIMER_4 9
#define PERSIST_KEY_ID_STOP_TIMER_FLAG 10
#define PERSIST_KEY_ID_STOP_TIMER_LAST_TIME 11

enum CustomFonts {
    font_big = 0,
    font_small,
    fonts_length
};
enum ApplicationWindow {
    main_window = 0,
    lap_timer_window,
    stop_timer_window,
    window_length
};

typedef uint8_t ApplicationMode;

Window *windows[window_length];
GFont *fonts[fonts_length];
ApplicationMode application_mode;

// common
char *set_clock_style();

// lap_timer.c
void load_lap_timer_window();

// stop_timer.c
void load_stop_timer_window();
