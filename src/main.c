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

static struct tm s_time;

static time_t stop_pointer[32];
static int32_t lap_counter = 0;

static void draw_active_timer(Layer *layer, GContext* ctx) {


  static char s_time_buffer[16];

  if (clock_is_24h_style()) {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M:%S", &s_time);
  } else {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M:%S", &s_time);
  }

  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, (GRect) {.origin = {0, 0}, .size = {144, 94}}, 0, GCornerNone);

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) { .origin = { 0, 2 }, .size = { 144, 46 } }, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], (GRect) { .origin = { 0, 2 }, .size = { 144, 46 } }, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

//  layer_set_frame(layer, (GRect) {.origin = {0, 0}, .size = {144, 44}});
//  text_layer_set_text(active_top_layer, s_time_buffer);
/*
  // load time now layer
  active_top_layer = text_layer_create((GRect) { .origin = { 0, 2 }, .size = { bounds.size.w, 46 } });
  text_layer_set_text_alignment(active_top_layer, GTextAlignmentCenter);
  text_layer_set_background_color(active_top_layer, GColorLightGray);
  text_layer_set_text_color(active_top_layer, GColorBlack);
  text_layer_set_font(active_top_layer, fonts[font_big]);

  layer_add_child(active_layer, text_layer_get_layer(active_top_layer));

  active_mid_left_layer = text_layer_create((GRect) { .origin = { 0, 47 }, .size = { (int16_t) (bounds.size.w / 2), line_height } });
  text_layer_set_text(active_mid_left_layer, "Lap");
  text_layer_set_text_alignment(active_mid_left_layer, GTextAlignmentCenter);
  text_layer_set_background_color(active_mid_left_layer, GColorLightGray);
  text_layer_set_text_color(active_mid_left_layer, GColorDarkGreen);
  text_layer_set_font(active_mid_left_layer, fonts[font_small]);

  layer_add_child(active_layer, text_layer_get_layer(active_mid_left_layer));

  active_mid_right_layer = text_layer_create((GRect) { .origin = { 72, 47 }, .size = { (int16_t) (bounds.size.w / 2), line_height } });
  text_layer_set_text(active_mid_right_layer, "Total");
  text_layer_set_text_alignment(active_mid_right_layer, GTextAlignmentCenter);
  text_layer_set_background_color(active_mid_right_layer, GColorLightGray);
  text_layer_set_text_color(active_mid_right_layer, GColorIslamicGreen);
  text_layer_set_font(active_mid_right_layer, fonts[font_small]);

  layer_add_child(active_layer, text_layer_get_layer(active_mid_right_layer));

  active_bottom_left_layer = text_layer_create((GRect) { .origin = { 0, 47 + 24 }, .size = { (int16_t) (bounds.size.w / 2), line_height } });
  text_layer_set_text(active_bottom_left_layer, "3/32");
  text_layer_set_text_alignment(active_bottom_left_layer, GTextAlignmentCenter);
  text_layer_set_background_color(active_bottom_left_layer, GColorLightGray);
  text_layer_set_text_color(active_bottom_left_layer, GColorOxfordBlue);
  text_layer_set_font(active_bottom_left_layer, fonts[font_small]);

  layer_add_child(active_layer, text_layer_get_layer(active_bottom_left_layer));

  active_bottom_right_layer = text_layer_create((GRect) { .origin = { 72, 47 + 24 }, .size = { (int16_t) (bounds.size.w / 2), line_height } });
  text_layer_set_text(active_bottom_right_layer, "04:23:45");
  text_layer_set_text_alignment(active_bottom_right_layer, GTextAlignmentCenter);
  text_layer_set_background_color(active_bottom_right_layer, GColorLightGray);
  text_layer_set_text_color(active_bottom_right_layer, GColorWindsorTan);
  text_layer_set_font(active_bottom_right_layer, fonts[font_small]);

  layer_add_child(active_layer, text_layer_get_layer(active_bottom_right_layer));
*/

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Update things here
  // Use a long-lived buffer

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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Length: %d", (int)lap_counter);
  for (int i = 0; i < lap_counter; ++i) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Length: %d", (int)stop_pointer[i]);
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Up button clicked");

  layer_set_frame(active_layer, (GRect) {.origin = {0, 90}, .size = {144, 20}});
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // stop this timer and save to array
//  text_layer_set_text(previous_left_layer, "Go Next Lap");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Down button clicked");
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
//  text_layer_set_text(previous_left_layer, "Clear all laps");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Down button clicked");

  lap_counter = 0;
  stop_pointer[0] = 0;
}

static void click_config_provider(void *context) {
  window_long_click_subscribe(BUTTON_ID_DOWN, 500, down_long_click_handler, NULL);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // constant
  const int8_t line_height = 32;
  const GPoint left_first = (GPoint) {.x = 0, .y = 4};
  const GPoint right_first = (GPoint) {.x = (int16_t) (bounds.size.w / 2), .y = 4};
  const GPoint left_second = (GPoint) {.x = 0, .y = line_height};
  const GPoint right_second = (GPoint) {.x = (int16_t) (bounds.size.w / 2), .y = line_height};

  // set up layers
  prev_layer = layer_create((GRect) {.origin = {0, 0}, .size = {bounds.size.w, line_height * 2}});
  next_layer = layer_create((GRect) {.origin = {0, 150}, .size = {bounds.size.w, 56}});
  active_layer = layer_create((GRect) {.origin = {0, 58}, .size = {bounds.size.w, 94}});

//  layer_set_hidden(prev_layer, true);
//  layer_set_hidden(next_layer, true);
//  layer_set_hidden(active_layer, true);

  layer_add_child(window_layer, prev_layer);
  layer_add_child(window_layer, next_layer);
  layer_add_child(window_layer, active_layer);

  // load font resource
  uint32_t resource_id = RESOURCE_ID_FONT_HANNA_32;

  for (uint8_t i = 0; i < fonts_length; ++i) {
    fonts[i] = fonts_load_custom_font(resource_get_handle(resource_id + i));
  }

  layer_set_update_proc(active_layer, draw_active_timer);

/*

  // load prev layer
  previous_left_layer = text_layer_create((GRect) {left_first, .size = {(int16_t) (bounds.size.w / 2), line_height } });
  text_layer_set_text(previous_left_layer, "02:10:00");
  text_layer_set_text_alignment(previous_left_layer, GTextAlignmentCenter);
  text_layer_set_background_color(previous_left_layer, GColorDarkGray);
  text_layer_set_text_color(previous_left_layer, GColorWhite);
  text_layer_set_font(previous_left_layer, fonts[font_small]);

  layer_add_child(prev_layer, text_layer_get_layer(previous_left_layer));

  previous_right_layer = text_layer_create((GRect) {right_first, .size = {(int16_t) (bounds.size.w / 2), line_height } });
  text_layer_set_text(previous_right_layer, "01:20:00");
  text_layer_set_text_alignment(previous_right_layer, GTextAlignmentCenter);
  text_layer_set_background_color(previous_right_layer, GColorDarkGray);
  text_layer_set_text_color(previous_right_layer, GColorLightGray);
  text_layer_set_font(previous_right_layer, fonts[font_small]);

  layer_add_child(prev_layer, text_layer_get_layer(previous_right_layer));

  previous_left_layer = text_layer_create((GRect) { left_second, .size = {(int16_t) (bounds.size.w / 2), line_height } });
  text_layer_set_text(previous_left_layer, "03:30:00");
  text_layer_set_text_alignment(previous_left_layer, GTextAlignmentCenter);
  text_layer_set_background_color(previous_left_layer, GColorDarkGray);
  text_layer_set_text_color(previous_left_layer, GColorWhite);
  text_layer_set_font(previous_left_layer, fonts[font_small]);

  layer_add_child(prev_layer, text_layer_get_layer(previous_left_layer));

  previous_right_layer = text_layer_create((GRect) { right_second, .size = {(int16_t) (bounds.size.w / 2), line_height } });
  text_layer_set_text(previous_right_layer, "00:44:23");
  text_layer_set_text_alignment(previous_right_layer, GTextAlignmentCenter);
  text_layer_set_background_color(previous_right_layer, GColorDarkGray);
  text_layer_set_text_color(previous_right_layer, GColorLightGray);
  text_layer_set_font(previous_right_layer, fonts[font_small]);

  layer_add_child(prev_layer, text_layer_get_layer(previous_right_layer));

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
    lap_counter = persist_read_int(PERSIST_KEY_ID_LAP_COUNTER);
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
