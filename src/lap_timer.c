//
// Created by Rogue Knight on 2015. 8. 2..
//

#include "common.h"

enum ActiveFlag {
    FLAG_UPPER,
    FLAG_NORMAL,
    FLAG_LOWER
};

static Layer *prev_layer;
static Layer *next_layer;
static Layer *active_layer;

static GRect bounds;

static const struct GSize active_layer_size = {144, 92};
static const struct GSize prev_layer_size = {144, 92};
static const struct GSize next_layer_size = {144, 92};

static const struct GPoint active_layer_origin_upper = {0, 15};
static const struct GPoint active_layer_origin_normal = {0, 38};
static const struct GPoint active_layer_origin_lower = {0, 61};
static const struct GPoint prev_layer_origin_normal = {0, 0};
static const struct GPoint next_layer_origin_normal = {0, 110};

static struct tm s_time;

static time_t stop_pointer[33];
static uint8_t lap_counter = 0;
static uint8_t pick_counter = 0;

static enum ActiveFlag active_flag = FLAG_NORMAL;

static void set_active_layer_position() {
  switch (active_flag) {
    case FLAG_UPPER:
      layer_set_frame(active_layer, (GRect) {.origin = active_layer_origin_upper, .size = active_layer_size});
      break;
    case FLAG_NORMAL:
      layer_set_frame(active_layer, (GRect) {.origin = active_layer_origin_normal, .size = active_layer_size});
      break;
    case FLAG_LOWER:
      layer_set_frame(active_layer, (GRect) {.origin = active_layer_origin_lower, .size = active_layer_size});
      break;
    default:
      break;
  }
}

static void set_active_flag(ButtonId buttonType) {
  switch (buttonType) {
    case BUTTON_ID_UP:
      if (lap_counter > 1) {
        if (lap_counter - pick_counter == 0) active_flag = FLAG_UPPER;
        else if (lap_counter - pick_counter > 0) active_flag = FLAG_NORMAL;
        else active_flag = FLAG_LOWER;
      } else {
        active_flag = FLAG_NORMAL;
      }

      break;
    case BUTTON_ID_SELECT:
      if (lap_counter > 1) active_flag = FLAG_LOWER;

      break;
    case BUTTON_ID_DOWN:
      if (lap_counter > 1) {
        if (pick_counter == 0) active_flag = FLAG_LOWER;
        else if (lap_counter - pick_counter > 0) active_flag = FLAG_NORMAL;
        else active_flag = FLAG_UPPER;
      } else {
        active_flag = FLAG_NORMAL;
      }

      break;

    default:break;
  }
}

static char *set_clock_style() {
  char *type;

  if (clock_is_24h_style()) {
    type = "%H:%M:%S";
  } else {
    type = "%I:%M:%S";
  }

  return type;
}

static void draw_active_timer(Layer *layer, GContext* ctx) {
  // declare vars and set geometry
  struct tm now;

  time_t stop_time;
  time_t diff_time;

  int hour, min, sec;

  char s_time_buffer[10];
  char lap_count_buffer[6];

  const struct GSize geo_active_layer_size = {(int16_t) (bounds.size.w / 2), 32};
  const struct GRect
      geo_active_top = {.origin = {bounds.origin.x, 2}, .size = {bounds.size.w, 46}},
      geo_active_text_left = {.origin = {bounds.origin.x, 44}, geo_active_layer_size},
      geo_active_text_right = {.origin = {(int16_t) (bounds.size.w / 2), 44}, geo_active_layer_size},
      geo_active_data_left = {.origin = {bounds.origin.x, 65}, geo_active_layer_size},
      geo_active_data_right = {.origin = {(int16_t) (bounds.size.w / 2), 65}, geo_active_layer_size};

  // fill background and set title
  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, (GRect) layer_get_bounds(layer), 0, GCornerNone);

  graphics_context_set_text_color(ctx, GColorMidnightGreen);
  graphics_draw_text(ctx, "LAP", fonts[font_small], geo_active_text_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  graphics_context_set_text_color(ctx, GColorOxfordBlue);
  graphics_draw_text(ctx, "TOTAL", fonts[font_small], geo_active_text_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // draw active point time
  stop_time = stop_pointer[(pick_counter ? lap_counter - pick_counter + 1 : pick_counter)];
  now = *localtime(&stop_time);

  strftime(s_time_buffer, sizeof(s_time_buffer), set_clock_style(), &now);

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_big], geo_active_top, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // draw lap
  snprintf(lap_count_buffer, sizeof(lap_count_buffer), "%d/%d", (int) pick_counter ? lap_counter - pick_counter : lap_counter, ARRAY_LENGTH(stop_pointer) - 1);

  graphics_context_set_text_color(ctx, GColorDarkGreen);
  graphics_draw_text(ctx, lap_count_buffer, fonts[font_small], geo_active_data_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // draw lap time
  if (!lap_counter) {
    snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", 0, 0, 0);
  }
  else {
    diff_time = stop_pointer[0] - stop_pointer[1];

    hour = (diff_time / 3600) % 24;
    min = (diff_time / 60) % 60;
    sec = diff_time % 60;

    snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);
  }

  graphics_context_set_text_color(ctx, GColorDukeBlue);
  graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_active_data_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}

static void draw_prev_timer(Layer *layer, GContext* ctx) {
  // declare vars and set geometry
  struct tm now;

  time_t stop_time;
  time_t diff_time;

  int hour, min, sec;

  char s_time_buffer[10];
  char lap_time_buffer[10];

  struct GSize geo_prev_layer_size = {(int16_t) (bounds.size.w / 2), 32};
  struct GRect
      geo_prev_data1_left = {.origin = {bounds.origin.x, 8}, geo_prev_layer_size},
      geo_prev_data1_right = {.origin = {(int16_t) (bounds.size.w / 2), 8}, geo_prev_layer_size},
      geo_prev_data3_left = {.origin = {bounds.origin.x, 3}, geo_prev_layer_size},
      geo_prev_data3_right = {.origin = {(int16_t) (bounds.size.w / 2), 3}, geo_prev_layer_size},
      geo_prev_data2_left = {.origin = {bounds.origin.x, 34}, geo_prev_layer_size},
      geo_prev_data2_right = {.origin = {(int16_t) (bounds.size.w / 2), 34}, geo_prev_layer_size};

  // fill background
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, (GRect) layer_get_bounds(layer), 0, GCornerNone);

  // draw prev point
  switch (active_flag) {
    case FLAG_NORMAL:
      // 1 line
      if (lap_counter && lap_counter - pick_counter > 0) {
        stop_time = stop_pointer[lap_counter - pick_counter];
        now = *localtime(&stop_time);

        strftime(s_time_buffer, sizeof(s_time_buffer), set_clock_style(), &now);

        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_prev_data1_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        if (lap_counter > 1) {
          diff_time = stop_pointer[lap_counter - pick_counter + 1] - stop_time;
        } else {
          diff_time = stop_pointer[0] - stop_pointer[1];
        }

        hour = (diff_time / 3600) % 24;
        min = (diff_time / 60) % 60;
        sec = diff_time % 60;

        snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

        graphics_context_set_text_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_prev_data1_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      break;
    case FLAG_LOWER:
      if (lap_counter > 1) {
        // 2 line
        // draw first prev point
        stop_time = stop_pointer[lap_counter - 1];
        now = *localtime(&stop_time);

        strftime(s_time_buffer, sizeof(s_time_buffer), set_clock_style(), &now);

        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_prev_data3_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        diff_time = stop_pointer[lap_counter] - stop_pointer[lap_counter - 1];

        hour = (diff_time / 3600) % 24;
        min = (diff_time / 60) % 60;
        sec = diff_time % 60;

        snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

        graphics_context_set_text_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_prev_data3_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        // horizon line
        graphics_context_set_stroke_color(ctx, GColorLightGray); graphics_draw_line(ctx, GPoint(0, 29), GPoint(144, 29));

        // draw second prev point
        stop_time = stop_pointer[lap_counter];
        now = *localtime(&stop_time);

        strftime(s_time_buffer, sizeof(s_time_buffer), set_clock_style(), &now);

        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_prev_data2_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        diff_time = stop_pointer[0] - stop_pointer[lap_counter];

        hour = (diff_time / 3600) % 24;
        min = (diff_time / 60) % 60;
        sec = diff_time % 60;

        snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

        graphics_context_set_text_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_prev_data2_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      break;
    case FLAG_UPPER:
      // 1 line clipping
      break;
    default:break;
  }
}

static void draw_next_timer(Layer *layer, GContext* ctx) {
  // declare vars and set geometry
  struct tm now;

  time_t stop_time;
  time_t diff_time;

  int hour, min, sec;

  char s_time_buffer[10];
  char lap_time_buffer[10];

  struct GSize geo_next_layer_size = {(int16_t) (bounds.size.w / 2), 32};
  struct GRect
      geo_next_data1_left = {.origin = {bounds.origin.x, 28}, geo_next_layer_size},
      geo_next_data1_right = {.origin = {(int16_t) (bounds.size.w / 2), 28}, geo_next_layer_size},
      geo_next_data3_left = {.origin = {bounds.origin.x, 1}, geo_next_layer_size},
      geo_next_data3_right = {.origin = {(int16_t) (bounds.size.w / 2), 1}, geo_next_layer_size},
      geo_next_data2_left = {.origin = {bounds.origin.x, 33}, geo_next_layer_size},
      geo_next_data2_right = {.origin = {(int16_t) (bounds.size.w / 2), 33}, geo_next_layer_size};

  // fill background
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, (GRect) layer_get_bounds(layer), 0, GCornerNone);

  // draw next point
  switch (active_flag) {
    case FLAG_NORMAL:
      // pick
      if (pick_counter && (lap_counter - pick_counter == 0)) {
        stop_time = stop_pointer[0];
        now = *localtime(&stop_time);

        strftime(s_time_buffer, sizeof(s_time_buffer), set_clock_style(), &now);

        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_next_data1_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        diff_time = stop_time - stop_pointer[lap_counter];

        hour = (diff_time / 3600) % 24;
        min = (diff_time / 60) % 60;
        sec = diff_time % 60;

        snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

        graphics_context_set_text_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_next_data1_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      } else if (pick_counter && (lap_counter - pick_counter > 0)) {
        if (pick_counter == 1) {
          stop_time = stop_pointer[0];
        } else {
          stop_time = stop_pointer[lap_counter - pick_counter + 2];
        }

        now = *localtime(&stop_time);

        strftime(s_time_buffer, sizeof(s_time_buffer), set_clock_style(), &now);

        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_next_data1_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        diff_time = stop_time - stop_pointer[lap_counter - pick_counter + 1];

        hour = (diff_time / 3600) % 24;
        min = (diff_time / 60) % 60;
        sec = diff_time % 60;

        snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

        graphics_context_set_text_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_next_data1_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      break;
    case FLAG_UPPER:
      if (lap_counter > 1) {
        // 2 line
        // draw first next point
        stop_time = stop_pointer[2];

        now = *localtime(&stop_time);

        strftime(s_time_buffer, sizeof(s_time_buffer), set_clock_style(), &now);

        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_next_data3_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        diff_time = stop_time - stop_pointer[1];

        hour = (diff_time / 3600) % 24;
        min = (diff_time / 60) % 60;
        sec = diff_time % 60;

        snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

        graphics_context_set_text_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_next_data3_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        // horizon line
        graphics_context_set_stroke_color(ctx, GColorLightGray); graphics_draw_line(ctx, GPoint(0, 27), GPoint(144, 27));

        // draw second next point
        if (lap_counter == 2) {
          stop_time = stop_pointer[0];
        } else {
          stop_time = stop_pointer[3];
        }

        now = *localtime(&stop_time);

        strftime(s_time_buffer, sizeof(s_time_buffer), set_clock_style(), &now);

        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_next_data2_left, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

        diff_time = stop_time - stop_pointer[2];

        hour = (diff_time / 3600) % 24;
        min = (diff_time / 60) % 60;
        sec = diff_time % 60;

        snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d:%02d", hour, min, sec);

        graphics_context_set_text_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, s_time_buffer, fonts[font_small], geo_next_data2_right, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      }
      break;
    case FLAG_LOWER:
      // 1 line clipping
      break;
    default:
      break;
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  s_time = *tick_time;

  stop_pointer[0] = mktime(&s_time);

  // update screen
  layer_mark_dirty(prev_layer);
  layer_mark_dirty(next_layer);
  layer_mark_dirty(active_layer);

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Uptime: %dh %dm %ds", s_time.tm_hour, s_time.tm_min, s_time.tm_sec);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  struct tm now = s_time;

  // update lap counter
  if (lap_counter < 32) {
    lap_counter++;

    stop_pointer[lap_counter] = mktime(&now);

    // reset pick counter
    pick_counter = 0;

    set_active_flag(click_recognizer_get_button_id(recognizer));
    set_active_layer_position();

//  layer_mark_dirty(prev_layer);
//  layer_mark_dirty(next_layer);
    layer_mark_dirty(active_layer);
  } else {
    // warning
    vibes_short_pulse();
  }
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // bind condition
  if (pick_counter < lap_counter) pick_counter++;

  set_active_flag(click_recognizer_get_button_id(recognizer));
  set_active_layer_position();

//  layer_mark_dirty(prev_layer);
//  layer_mark_dirty(next_layer);
  layer_mark_dirty(active_layer);
}


static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clear this lap: %d / %d", pick_counter, lap_counter);

  const uint32_t const very_short[] = { 50 };

  VibePattern very_short_pattern = {
      .durations = very_short,
      .num_segments = ARRAY_LENGTH(very_short),
  };

  if (lap_counter && pick_counter) {
    // ReArrange Array
    for (int i = lap_counter - pick_counter + 1; i < lap_counter; ++i) {
      stop_pointer[i] = stop_pointer[i + 1];
    }

    // reduce lap count and pick
    lap_counter--;
    pick_counter--;

    if (lap_counter == pick_counter) set_active_flag(BUTTON_ID_UP);
    else set_active_flag(BUTTON_ID_DOWN);

    set_active_layer_position();

    vibes_short_pulse();
  } else {
    vibes_enqueue_custom_pattern(very_short_pattern);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // bind condition
  if (pick_counter > 0) pick_counter--;

  set_active_flag(click_recognizer_get_button_id(recognizer));
  set_active_layer_position();

//  layer_mark_dirty(prev_layer);
//  layer_mark_dirty(next_layer);
  layer_mark_dirty(active_layer);
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "Clear all laps");

  lap_counter = 0;
  pick_counter = 0;
  stop_pointer[0] = 0;

  active_flag = FLAG_NORMAL;

  set_active_layer_position();
  vibes_double_pulse();

//  layer_mark_dirty(prev_layer);
//  layer_mark_dirty(next_layer);
  layer_mark_dirty(active_layer);
}

static void click_config_provider(void *context) {
  uint16_t ms_long_delay = 500;

  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);

  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, ms_long_delay, select_long_click_handler, NULL);

  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_long_click_subscribe(BUTTON_ID_DOWN, ms_long_delay, down_long_click_handler, NULL);
}

static void window_load(Window *window) {
  if (persist_exists(PERSIST_KEY_ID_LAPS)) {
    persist_read_data(PERSIST_KEY_ID_LAPS, &stop_pointer, sizeof(stop_pointer));
  }

  if (persist_exists(PERSIST_KEY_ID_LAP_COUNTER)) {
    lap_counter = (uint8_t) persist_read_int(PERSIST_KEY_ID_LAP_COUNTER);
  }

  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);

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
  layer_set_update_proc(next_layer, draw_next_timer);

  layer_set_hidden(prev_layer, false);
  layer_set_hidden(next_layer, false);
  layer_set_hidden(active_layer, false);

  // move active layer by condition
  set_active_flag(BUTTON_ID_SELECT);
  set_active_layer_position();

  // bind tick service
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void window_unload(Window *window) {
  persist_write_data(PERSIST_KEY_ID_LAPS, &stop_pointer, sizeof(stop_pointer));
  persist_write_int(PERSIST_KEY_ID_LAP_COUNTER, (int32_t)lap_counter);

  tick_timer_service_unsubscribe();

  layer_destroy(prev_layer);
  layer_destroy(next_layer);
  layer_destroy(active_layer);
}

void load_lap_timer_window(Window *window) {
  const bool animated = true;

  window_set_background_color(window, GColorDarkGray);

  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
  });

  window_stack_push(window, animated);
}

