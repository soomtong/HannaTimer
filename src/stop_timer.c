//
// Created by Rogue Knight on 2015. 8. 2..
//

#include "common.h"

static Layer *grid_layer;

static GRect bounds;

static const struct GSize active_layer_size = {144, 35};
static const struct GPoint active_layer_origin[5] = {
    {0, 0},
    {0, 28},
    {0, 66},
    {0, 90},
    {0, 120}
};
static const struct GSize layer_size = {144, 30};
static const struct GPoint layer_origin[5] = {
    {0, -4},
    {0, 29},
    {0, 62},
    {0, 95},
    {0, 128}
};

static struct tm s_time;

static uint8_t pick_counter = 2;
static time_t stop_timer[5];
static bool active_timer[5];

static void draw_stop_timer(Layer *layer, GContext* ctx) {
  int hour, min, sec;

  char s_time_buffer[10];

  // fill background
  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, (GRect) {.origin = active_layer_origin[2], .size = active_layer_size}, 0, GCornerNone);

  hour = (stop_timer[2] / 3600) % 24;
  min = (stop_timer[2] / 60) % 60;
  sec = stop_timer[2] % 60;

  snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) {.origin = layer_origin[0], .size = layer_size}, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) {.origin = layer_origin[1], .size = layer_size}, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) {.origin = layer_origin[2], .size = layer_size}, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) {.origin = layer_origin[3], .size = layer_size}, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) {.origin = layer_origin[4], .size = layer_size}, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  s_time = *tick_time;

  // update screen
  // is there active timer?
  active_timer[2] = true;
  stop_timer[2]++;
  layer_mark_dirty(grid_layer);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer: %ds", (int)stop_timer[2]);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "up click");

  if (pick_counter < 4) pick_counter++;
}

static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "change window");

  // load other window
  load_lap_timer_window();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "select click, now pick: %d", pick_counter);

}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "select long click for clear, now pick: %d", pick_counter);

}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "down click, now pick: %d", pick_counter);

  if (pick_counter > 0) pick_counter--;
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "down long click for all clear");

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
  if (persist_exists(PERSIST_KEY_ID_STOP_PICK_COUNTER)) {
    pick_counter = (uint8_t) persist_read_int(PERSIST_KEY_ID_STOP_PICK_COUNTER);
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
  persist_write_int(PERSIST_KEY_ID_STOP_PICK_COUNTER, (int32_t)pick_counter);

  tick_timer_service_unsubscribe();

  layer_destroy(grid_layer);
}

void load_stop_timer_window() {
  const bool animated = true;
  application_mode = stop_timer_window;

  window_set_background_color(windows[stop_timer_window], GColorDarkGray);

  window_set_click_config_provider(windows[stop_timer_window], click_config_provider);
  window_set_window_handlers(windows[stop_timer_window], (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });

  window_stack_remove(windows[lap_timer_window], animated);
  window_stack_push(windows[stop_timer_window], animated);
}

