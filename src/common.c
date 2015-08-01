//
// Created by Rogue Knight on 2015. 8. 2..
//

#include "common.h"

char *set_clock_style() {
  char *type;

  if (clock_is_24h_style()) {
    type = "%H:%M:%S";
  } else {
    type = "%I:%M:%S";
  }

  return type;
}
