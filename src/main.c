#include <pebble.h>

#define PERSIST_KEY_ID_LAPS 1
#define PERSIST_KEY_ID_LAP_COUNTER 2

enum CustomFonts {
  font_big=0,
  font_small,
  fonts_length
};

static Window *window;
static Layer *prev_layer;
static Layer *next_layer;
static Layer *active_layer;

static GFont *fonts[fonts_length];
static GRect bounds;

static const struct GPoint active_layer_origin_normal = {0, 38};
static const struct GPoint active_layer_origin_upper = {0, 18};
static const struct GPoint active_layer_origin_lower = {0, 58};
static const struct GSize active_layer_size = {144, 92};
static const struct GPoint next_layer_origin_normal = {0, 150};
static const struct GPoint next_layer_origin_upper = {0, 130};
static const struct GPoint next_layer_origin_lower = {0, 170};
static const struct GSize next_layer_size = {144, 92};
static const struct GPoint prev_layer_origin_normal = {0, 0};
static const struct GPoint prev_layer_origin_upper = {0, -20};
static const struct GPoint prev_layer_origin_lower = {0, 20};
static const struct GSize prev_layer_size = {144, 92};

static struct tm s_time;

static time_t stop_pointer[32];
static uint8_t lap_counter = 0;
static uint8_t pick_counter = 0;

static void draw_active_timer(Layer *layer, GContext* ctx) {

  const struct GRect
      geo_active_top = {.origin = {bounds.origin.x, 2}, .size = {bounds.size.w, 46}},
      geo_active_text_left = {.origin = {bounds.origin.x, 44}, .size = {(int16_t) (bounds.size.w / 2), 32}},
      geo_active_text_right = {.origin = {(int16_t) (bounds.size.w / 2), 44}, .size = {(int16_t) (bounds.size.w / 2), 32}},
      geo_active_data_left = {.origin = {bounds.origin.x, 65}, .size = {(int16_t) (bounds.size.w / 2), 32}},
      geo_active_data_right = {.origin = {(int16_t) (bounds.size.w / 2), 65}, .size = {(int16_t) (bounds.size.w / 2), 32}};

  struct tm temp_time = s_time;

  char s_time_buffer[10];
  char total_laps_buffer[10];
  char lap_count_buffer[6];

  int hour, min, sec;

  time_t total_diff;
  time_t now = mktime(&temp_time);

  if (clock_is_24h_style()) {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M:%S", &s_time);
  } else {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M:%S", &s_time);
  }

//  if (layer_get_hidden(layer)) layer_set_hidden(layer, false);

  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, (GRect) layer_get_bounds(layer), 0, GCornerNone);

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], geo_active_top, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorIslamicGreen);
  graphics_draw_text(ctx, "LAP", fonts[font_small], geo_active_text_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorOxfordBlue);
  graphics_draw_text(ctx, "TOTAL", fonts[font_small], geo_active_text_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  snprintf(lap_count_buffer, sizeof(lap_count_buffer), "%d/%d", (int) lap_counter, sizeof(stop_pointer) / sizeof(time_t));

  graphics_context_set_text_color(ctx, GColorDarkGreen);
  graphics_draw_text(ctx, lap_count_buffer, fonts[font_small], geo_active_data_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  if (stop_pointer[0]) {
    total_diff = now - stop_pointer[0];

    hour = (total_diff / 3600) % 24;
    min = (total_diff / 60) % 60;
    sec = total_diff % 60;

    snprintf(total_laps_buffer, sizeof(total_laps_buffer), "%02d:%02d:%02d", hour, min, sec);
  } else {
    snprintf(total_laps_buffer, sizeof(total_laps_buffer), "%02d:%02d:%02d", 0, 0, 0);
  }

  graphics_context_set_text_color(ctx, GColorDukeBlue);
  graphics_draw_text(ctx, total_laps_buffer, fonts[font_small], geo_active_data_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void draw_prev_timer(Layer *layer, GContext* ctx) {

  struct tm temp_time = s_time;
  struct tm *stop_time;

  time_t temp_stop_time;
  time_t total_diff;
  time_t now = mktime(&temp_time);

  char stop_time_buffer[10];

  int hour, min, sec;

  struct GSize prev_layer_size = {(int16_t) (bounds.size.w / 2), 32};
  struct GRect
      geo_prev_data1_left = {.origin = {bounds.origin.x, 7}, prev_layer_size},
      geo_prev_data1_right = {.origin = {(int16_t) (bounds.size.w / 2), 7}, prev_layer_size},
      geo_prev_data3_left = {.origin = {bounds.origin.x, 3}, prev_layer_size},
      geo_prev_data3_right = {.origin = {(int16_t) (bounds.size.w / 2), 3}, prev_layer_size},
      geo_prev_data2_left = {.origin = {bounds.origin.x, 32}, prev_layer_size},
      geo_prev_data2_right = {.origin = {(int16_t) (bounds.size.w / 2), 32}, prev_layer_size};

  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, (GRect) layer_get_bounds(layer), 0, GCornerNone);

  if (lap_counter == 1) {

    temp_stop_time = stop_pointer[0];

    stop_time = localtime(&temp_stop_time);
    total_diff = now - stop_pointer[0];

    hour = (total_diff / 3600) % 24;
    min = (total_diff / 60) % 60;
    sec = total_diff % 60;

    strftime(stop_time_buffer, sizeof(stop_time_buffer), "%H:%M:%S", stop_time);

    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, stop_time_buffer, fonts[font_small], geo_prev_data1_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    snprintf(stop_time_buffer, sizeof(stop_time_buffer), "%02d:%02d:%02d", hour, min, sec);

    graphics_context_set_text_color(ctx, GColorLightGray);
    graphics_draw_text(ctx, stop_time_buffer, fonts[font_small], geo_prev_data1_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  } else if (lap_counter > 1) {

    temp_stop_time = stop_pointer[lap_counter - 2];

    stop_time = localtime(&temp_stop_time);
    total_diff = stop_pointer[lap_counter - 1] - stop_pointer[lap_counter - 2];

    hour = (total_diff / 3600) % 24;
    min = (total_diff / 60) % 60;
    sec = total_diff % 60;

    strftime(stop_time_buffer, sizeof(stop_time_buffer), "%H:%M:%S", stop_time);

    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, stop_time_buffer, fonts[font_small], geo_prev_data3_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    snprintf(stop_time_buffer, sizeof(stop_time_buffer), "%02d:%02d:%02d", hour, min, sec);

    graphics_context_set_text_color(ctx, GColorLightGray);
    graphics_draw_text(ctx, stop_time_buffer, fonts[font_small], geo_prev_data3_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    // horizon line
    graphics_context_set_stroke_color(ctx, GColorLightGray); graphics_draw_line(ctx, GPoint(0, 28), GPoint(144, 28));

    temp_stop_time = stop_pointer[lap_counter - 1];

    stop_time = localtime(&temp_stop_time);
    total_diff = now - stop_pointer[lap_counter - 1];

    hour = (total_diff / 3600) % 24;
    min = (total_diff / 60) % 60;
    sec = total_diff % 60;

    strftime(stop_time_buffer, sizeof(stop_time_buffer), "%H:%M:%S", stop_time);

    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, stop_time_buffer, fonts[font_small], geo_prev_data2_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    snprintf(stop_time_buffer, sizeof(stop_time_buffer), "%02d:%02d:%02d", hour, min, sec);

    graphics_context_set_text_color(ctx, GColorLightGray);
    graphics_draw_text(ctx, stop_time_buffer, fonts[font_small], geo_prev_data2_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    layer_set_frame(active_layer, (GRect) { .origin = active_layer_origin_lower, .size = active_layer_size });
   }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  s_time = *tick_time;

  // update screen
  layer_mark_dirty(active_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
//  text_layer_set_text(previous_left_layer, "Select");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Select button clicked");

  stop_pointer[lap_counter] = mktime(&s_time);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Uptime: %dh %dm %ds", s_time.tm_hour, s_time.tm_min, s_time.tm_sec);

  lap_counter++;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "pick counter: %d", (int)pick_counter);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Length: %d", (int)lap_counter);
  for (int i = 0; i < lap_counter; ++i) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Length: %d", (int)stop_pointer[i]);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Up button clicked");

  if (pick_counter < lap_counter) pick_counter++;

  layer_set_frame(active_layer, (GRect) {.origin = active_layer_origin_lower, .size = active_layer_size});
  layer_set_frame(next_layer, (GRect) {.origin = next_layer_origin_lower, .size = next_layer_size});
  layer_set_frame(prev_layer, (GRect) {.origin = prev_layer_origin_lower, .size = prev_layer_size});
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Down button clicked");

  if (pick_counter > 0) pick_counter--;

  layer_set_frame(active_layer, (GRect) {.origin = active_layer_origin_upper, .size = active_layer_size});
  layer_set_frame(next_layer, (GRect) {.origin = next_layer_origin_upper, .size = next_layer_size});
  layer_set_frame(prev_layer, (GRect) {.origin = prev_layer_origin_upper, .size = prev_layer_size});
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
//  text_layer_set_text(previous_left_layer, "Clear all laps");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Down button clicked");

  lap_counter = 0;
  pick_counter = 0;
  stop_pointer[0] = 0;

  layer_set_frame(active_layer, (GRect) {.origin = active_layer_origin_normal, .size = active_layer_size});
  layer_set_frame(next_layer, (GRect) {.origin = next_layer_origin_normal, .size = next_layer_size});
  layer_set_frame(prev_layer, (GRect) {.origin = prev_layer_origin_normal, .size = prev_layer_size});
}

static void click_config_provider(void *context) {
  window_long_click_subscribe(BUTTON_ID_DOWN, 500, down_long_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);

  // load font resource
  uint32_t resource_id = RESOURCE_ID_FONT_HANNA_32;

  for (uint8_t i = 0; i < fonts_length; ++i) {
    fonts[i] = fonts_load_custom_font(resource_get_handle(resource_id + i));
  }

  // set up layers
  prev_layer = layer_create((GRect) {.origin = prev_layer_origin_normal, .size = prev_layer_size});
  next_layer = layer_create((GRect) {.origin = next_layer_origin_normal, .size = next_layer_size});
  active_layer = layer_create((GRect) {.origin = active_layer_origin_normal, .size = active_layer_size});

  layer_set_hidden(prev_layer, true);
  layer_set_hidden(next_layer, true);
  layer_set_hidden(active_layer, true);

  layer_add_child(window_layer, prev_layer);
  layer_add_child(window_layer, next_layer);
  layer_add_child(window_layer, active_layer);

  layer_set_update_proc(active_layer, draw_active_timer);
  layer_set_update_proc(prev_layer, draw_prev_timer);

  layer_set_hidden(prev_layer, false);
  layer_set_hidden(next_layer, false);
  layer_set_hidden(active_layer, false);


/*

  // load prev layer


  // load next layer
  next_left_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { (int16_t) (bounds.size.w / 2), 18 } });
  text_layer_set_text(next_left_layer, "03:30:00");
  text_layer_set_text_alignment(next_left_layer, GTextAlignmentCenter);
  text_layer_set_background_color(next_left_layer, GColorBlack);
  text_layer_set_text_color(next_left_layer, GColorWhite);
  text_layer_set_font(next_left_layer, fonts[font_small]);

  layer_add_child(next_layer, text_layer_get_layer(next_left_layer));

  next_right_layer = text_layer_create((GRect) { .origin = { 71, 0 }, .size = { (int16_t) (bounds.size.w / 2), 18 } });
  text_layer_set_text(next_right_layer, "01:20:00");
  text_layer_set_text_alignment(next_right_layer, GTextAlignmentCenter);
  text_layer_set_background_color(next_right_layer, GColorBlack);
  text_layer_set_text_color(next_right_layer, GColorLightGray);
  text_layer_set_font(next_right_layer, fonts[font_small]);

  layer_add_child(next_layer, text_layer_get_layer(next_right_layer));


*/

  // bind tick service
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void window_unload(Window *window) {
  tick_timer_service_unsubscribe();

  layer_destroy(prev_layer);
  layer_destroy(next_layer);
  layer_destroy(active_layer);

  for (uint8_t i = 0; i < fonts_length; ++i) {
    if (fonts[i]) fonts_unload_custom_font(fonts[i]);
  }
}

static void init(void) {
  // load previous persistent data
  if (persist_exists(PERSIST_KEY_ID_LAPS)) {
    persist_read_data(PERSIST_KEY_ID_LAPS, &stop_pointer, sizeof(stop_pointer));
  }
  if (persist_exists(PERSIST_KEY_ID_LAP_COUNTER)) {
    lap_counter = (uint8_t) persist_read_int(PERSIST_KEY_ID_LAP_COUNTER);
  }

  // prepare main window
  window = window_create();

  window_set_background_color(window, GColorDarkGray);

  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  // save persistent data
  persist_write_data(PERSIST_KEY_ID_LAPS, &stop_pointer, sizeof(stop_pointer));
  persist_write_int(PERSIST_KEY_ID_LAP_COUNTER, (int32_t)lap_counter);

  window_destroy(window);
}

int main(void) {
  init();

  app_event_loop();

  deinit();

  return 0;
}
