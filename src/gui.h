#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "rival.h"

bool gui_setup();
void *gui_main(void *data);
void gui_queue(void (*f)(void *data), void *data);
void gui_set_title(const char *title);

typedef void gui_handler_color(uint8_t r, uint8_t g, uint8_t b);
void gui_register_handler_color(gui_handler_color *h);

typedef void gui_handler_mode(RIVAL_LIGHT_MODE mode);
void gui_register_handler_mode(gui_handler_mode *h);

typedef void gui_handler_dpi(RIVAL_DPI_PRESET preset, RIVAL_DPI dpi);
void gui_register_handler_dpi(gui_handler_dpi *h);

typedef void gui_handler_rate(RIVAL_RATE rate);
void gui_register_handler_rate(gui_handler_rate *h);

typedef void gui_handler_close();
void gui_register_handler_close(gui_handler_close *h);
