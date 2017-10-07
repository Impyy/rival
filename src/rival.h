#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <libudev.h>

typedef enum RIVAL_LIGHT_MODE {
	RIVAL_LIGHT_MODE_STATIC,
	RIVAL_LIGHT_MODE_BREATHE_SLOW,
	RIVAL_LIGHT_MODE_BREATHE_MEDIUM,
	RIVAL_LIGHT_MODE_BREATHE_FAST,
	RIVAL_LIGHT_MODE_OFF // not actually a valid mode
} RIVAL_LIGHT_MODE;

typedef enum RIVAL_DPI {
	RIVAL_DPI_4000,
	RIVAL_DPI_2000,
	RIVAL_DPI_1750,
	RIVAL_DPI_1500,
	RIVAL_DPI_1250,
	RIVAL_DPI_1000,
	RIVAL_DPI_500,
	RIVAL_DPI_250
} RIVAL_DPI;

typedef enum RIVAL_RATE {
	RIVAL_RATE_1000,
	RIVAL_RATE_500,
	RIVAL_RATE_250,
	RIVAL_RATE_125
} RIVAL_RATE;

typedef enum RIVAL_DPI_PRESET {
	RIVAL_DPI_PRESET1,
	RIVAL_DPI_PRESET2
} RIVAL_DPI_PRESET;

typedef void rival_handler_opened();
typedef void rival_handler_closed();

struct rival {
	int fd;
	struct udev *udev;
	rival_handler_opened *on_opened;
	rival_handler_closed *on_closed;
};

struct rival *rival_new(rival_handler_opened *opened, rival_handler_closed *closed);
void rival_free(struct rival *rival);
void rival_monitor(struct rival *rival, bool *end);
bool rival_get_name(struct rival *rival, uint8_t *buffer, size_t buffer_size);
bool rival_set_light_color(struct rival *rival, uint8_t r, uint8_t g, uint8_t b);
bool rival_set_light_mode(struct rival *rival, RIVAL_LIGHT_MODE mode);
bool rival_set_dpi(struct rival *rival, RIVAL_DPI_PRESET preset, RIVAL_DPI dpi);
bool rival_set_rate(struct rival *rival, RIVAL_RATE rate);
bool rival_save(struct rival *rival);
