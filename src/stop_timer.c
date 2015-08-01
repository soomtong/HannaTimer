//
// Created by Rogue Knight on 2015. 8. 2..
//

#include "common.h"

//static Layer *grid_layer;

static GRect bounds;

static struct tm s_time;


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  s_time = *tick_time;

  // update screen

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Uptime: %dh %dm %ds", s_time.tm_hour, s_time.tm_min, s_time.tm_sec);
}

static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "change window");

  window_stack_remove(windows[stop_timer_window], false);
  load_lap_timer_window();
}

static void click_config_provider(void *context) {
  uint16_t ms_long_delay = 500;

  window_long_click_subscribe(BUTTON_ID_UP, ms_long_delay, up_long_click_handler, NULL);
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);


  // bind tick service
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void window_unload(Window *window) {
  tick_timer_service_unsubscribe();

  application_mode = stop_timer_window;
}

void load_stop_timer_window() {
  Window *window = windows[stop_timer_window];

  const bool animated = true;

  window_set_background_color(window, GColorDarkGray);

  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });

  window_stack_push(window, animated);
}

