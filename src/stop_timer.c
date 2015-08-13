//
// Created by Rogue Knight on 2015. 8. 2..
//

#include "common.h"

static Layer *grid_layer;

static GRect bounds;

static const struct GSize active_layer_size = {144, 36};
static const struct GPoint active_layer_origin[5] = {
    {0, 0},
    {0, 33},
    {0, 66},
    {0, 99},
    {0, 132}
};
static const struct GSize layer_size = {144, 30};
static const struct GPoint layer_origin[5] = {
    {0, -4},
    {0, 29},
    {0, 62},
    {0, 95},
    {0, 128}
};

static uint8_t pick_counter = 2;

static time_t stop_timer[STOP_TIMER_SIZE];
static bool active_timer[STOP_TIMER_SIZE];

static void draw_stop_timer(Layer *layer, GContext* ctx) {
  int hour, min, sec;

  char s_time_buffer[10];

  for (uint8_t i = 0; i < STOP_TIMER_SIZE; ++i) {
    hour = (stop_timer[i] / 3600) % 24;
    min = (stop_timer[i] / 60) % 60;
    sec = stop_timer[i] % 60;

    snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

    if (i == pick_counter) {
      // fill background
      graphics_context_set_fill_color(ctx, GColorLightGray);
      graphics_fill_rect(ctx, (GRect) {.origin = active_layer_origin[i], .size = active_layer_size}, 0, GCornerNone);

      if (active_timer[i]) {
        graphics_context_set_text_color(ctx, GColorBlack);
      } else {
        graphics_context_set_text_color(ctx, GColorVeryLightBlue);
      }

      graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) {.origin = layer_origin[i], .size = layer_size}, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    } else {
      // normal timer background
      if (active_timer[i]) {
        graphics_context_set_text_color(ctx, GColorWhite);
      } else {
        if (stop_timer[i]) {
          graphics_context_set_text_color(ctx, GColorLightGray);
        } else {
          graphics_context_set_text_color(ctx, GColorOxfordBlue);
        }
      }

      graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) {.origin = layer_origin[i], .size = layer_size}, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    }
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // conditional update
  bool need_update = false;

  // update screen
  for (uint8_t i = 0; i < STOP_TIMER_SIZE; ++i) {
    if (active_timer[i]) {
      stop_timer[i]++;

      need_update = true;
    }
  }

  if (need_update) {
    layer_mark_dirty(grid_layer);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (pick_counter > 0) {
    pick_counter--;
    layer_mark_dirty(grid_layer);
  }
}

static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  // load other window
  load_lap_timer_window();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  const uint32_t const very_short[] = { 50 };

  VibePattern very_short_pattern = {
      .durations = very_short,
      .num_segments = ARRAY_LENGTH(very_short),
  };

  // toggle this timer
  active_timer[pick_counter] = !active_timer[pick_counter];

  if (!active_timer[pick_counter]) {
    vibes_enqueue_custom_pattern(very_short_pattern);
  }

  layer_mark_dirty(grid_layer);
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  // reset this timer
  active_timer[pick_counter] = false;
  stop_timer[pick_counter] = 0;

  vibes_short_pulse();

  layer_mark_dirty(grid_layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // move down navigator
  if (pick_counter < 4) {
    pick_counter++;
    layer_mark_dirty(grid_layer);
  }
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  // reset all timer
  for (int i = 0; i < STOP_TIMER_SIZE; ++i) {
    active_timer[i] = false;
    stop_timer[i] = 0;
    pick_counter = 2;
  }

  vibes_double_pulse();

  layer_mark_dirty(grid_layer);
}

static void click_config_provider(void *context) {
  uint16_t ms_long_delay = 500;

  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_long_click_subscribe(BUTTON_ID_UP, ms_long_delay, up_long_click_handler, NULL);

  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, ms_long_delay, select_long_click_handler, NULL);

  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, ms_long_delay, down_long_click_handler, NULL);
}

static void window_load(Window *window) {
  int flag = 0;
  time_t now = 0, saved = 0, diff = 0;

  if (persist_exists(PERSIST_KEY_ID_STOP_TIMER)) {
    persist_read_data(PERSIST_KEY_ID_STOP_TIMER, &stop_timer, sizeof(stop_timer));
  }

  if (persist_exists(PERSIST_KEY_ID_STOP_TIMER_LAST_TIME)) {
    now = time(NULL);
    persist_read_data(PERSIST_KEY_ID_STOP_TIMER_LAST_TIME, &saved, sizeof(saved));

    diff = now - saved;   // diff = (time_t) difftime(saved, now);
  }

  if (persist_exists(PERSIST_KEY_ID_STOP_TIMER_FLAG)) {
    flag = persist_read_int(PERSIST_KEY_ID_STOP_TIMER_FLAG);
  }

  if (persist_exists(PERSIST_KEY_ID_STOP_PICK_COUNTER)) {
    pick_counter = (uint8_t) persist_read_int(PERSIST_KEY_ID_STOP_PICK_COUNTER);
  }

/*
  APP_LOG(APP_LOG_LEVEL_DEBUG, "saved flag: %d", flag);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "diff time: %d", (int) diff);

  for (int j = 0; j < STOP_TIMER_SIZE; ++j) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "saved time: %d", (int) stop_timer[j]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "saved stat: %d", (int) active_timer[j]);
  }
*/

  // load flag bit
  // 31 = 11111
  // 30 = 11110
  // 15 = 01111
  // 4 = 00100
  // 2 = 00010
  // 1 = 00001

  for (int i = 0; i < STOP_TIMER_SIZE; ++i) {
    active_timer[i] = (bool) (flag & (1 << i));

    if (active_timer[i]) {
      stop_timer[i] = stop_timer[i] + diff;
    }
  }

  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);

  grid_layer = layer_create(bounds);

  layer_set_hidden(grid_layer, true);
  layer_add_child(window_layer, grid_layer);

  layer_set_update_proc(grid_layer, draw_stop_timer);
  layer_set_hidden(grid_layer, false);

  // bind tick service
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void window_unload(Window *window) {
  // save flag bit
  int flag = 0;

  for (int i = 0; i < STOP_TIMER_SIZE; ++i) {
    if (active_timer[i]) flag = flag + (1 << i);
  }

  time_t now = time(NULL);

  persist_write_data(PERSIST_KEY_ID_STOP_TIMER, &stop_timer, sizeof(stop_timer));
  persist_write_data(PERSIST_KEY_ID_STOP_TIMER_LAST_TIME, &now, sizeof(now));
  persist_write_int(PERSIST_KEY_ID_STOP_TIMER_FLAG, flag);
  persist_write_int(PERSIST_KEY_ID_STOP_PICK_COUNTER, (int32_t)pick_counter);

  tick_timer_service_unsubscribe();

  layer_destroy(grid_layer);
}

void load_stop_timer_window() {
  const bool animated = true;
  application_mode = stop_timer_window;

//  APP_LOG(APP_LOG_LEVEL_DEBUG, "app mode = %d", application_mode);

  window_set_background_color(windows[stop_timer_window], GColorDarkGray);

  window_set_click_config_provider(windows[stop_timer_window], click_config_provider);
  window_set_window_handlers(windows[stop_timer_window], (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });

  window_stack_remove(windows[lap_timer_window], animated);
  window_stack_push(windows[stop_timer_window], animated);
}

