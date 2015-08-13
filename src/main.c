#include "common.h"

static void switch_app_window(uint8_t app_mode) {
  switch (app_mode) {
    case lap_timer_window:
      load_lap_timer_window();
      break;

    case stop_timer_window:
      load_stop_timer_window();
      break;

    default:
      break;
  }
}

static void init(void) {
  // load previous persistent data
  if (persist_exists(PERSIST_KEY_ID_MODE) == lap_timer_window || persist_exists(PERSIST_KEY_ID_MODE) == stop_timer_window) {
    application_mode = (uint8_t) persist_read_int(PERSIST_KEY_ID_MODE);
  } else {
    application_mode = (uint8_t) lap_timer_window;
  }

  // prepare main window
  for (int i = 0; i < window_length; ++i) {
    windows[i] = window_create();
  }

  // load font resource
  uint32_t resource_id = RESOURCE_ID_FONT_HANNA_32;

  for (uint8_t i = 0; i < fonts_length; ++i) {
    fonts[i] = fonts_load_custom_font(resource_get_handle(resource_id + i));
  }

//  window_stack_push(windows[main_window], false);

  switch_app_window(application_mode);
}

static void deinit(void) {
  // save previous mode
  persist_write_int(PERSIST_KEY_ID_MODE, (int32_t) application_mode);

  // clear all
/*
  for (int j = PERSIST_KEY_ID_MODE; j < PERSIST_KEY_ID_STOP_TIMER_LAST_TIME; ++j) {
    persist_delete(j);
  }
*/

  for (uint8_t i = 0; i < fonts_length; ++i) {
    if (fonts[i]) fonts_unload_custom_font(fonts[i]);
  }

  for (int i = 0; i < window_length; ++i) {
    if (windows[i]) window_destroy(windows[i]);
  }
}

int main(void) {
  init();

  app_event_loop();

  deinit();

  return 0;
}
