#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "rival.h"

typedef void gui_handler_color(uint8_t r, uint8_t g, uint8_t b);
typedef void gui_handler_mode(RIVAL_LIGHT_MODE mode);
typedef void gui_handler_dpi(RIVAL_DPI_PRESET preset, RIVAL_DPI dpi);
typedef void gui_handler_rate(RIVAL_RATE rate);
typedef void gui_handler_close();
typedef void gui_handler_save();
typedef void gui_handler_apply();

struct gui_model {
	// pointers to config entries
	RIVAL_DPI *dpi_preset1;
	RIVAL_DPI *dpi_preset2;
	RIVAL_RATE *rate;
	RIVAL_LIGHT_MODE *light_mode;
	uint8_t *color_r;
	uint8_t *color_g;
	uint8_t *color_b;
	bool *enable_preview;

	// handlers
	gui_handler_color *handler_color;
	gui_handler_mode *handler_mode;
	gui_handler_dpi *handler_dpi;
	gui_handler_rate *handler_rate;
	gui_handler_close *handler_close;
	gui_handler_save *handler_save;
	gui_handler_apply *handler_apply;
};

bool gui_setup();
void *gui_main(void *data);
void gui_queue(void (*f)(void *data), void *data);
void gui_set_title(char *title);
void gui_set_title_heap(char *title);
void gui_set_model(struct gui_model *model);
void gui_on_rival_opened();
void gui_on_rival_closed();
