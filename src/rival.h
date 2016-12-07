#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum RIVAL_LIGHT_MODE {
	RIVAL_LIGHT_MODE_STATIC = 0x01,
	RIVAL_LIGHT_MODE_BREATHE_SLOW = 0x02,
	RIVAL_LIGHT_MODE_BREATHE_MEDIUM = 0x03,
	RIVAL_LIGHT_MODE_BREATHE_FAST = 0x04,
	RIVAL_LIGHT_MODE_OFF = 0x05 // not actually a valid mode
} RIVAL_LIGHT_MODE;

typedef enum RIVAL_DPI {
	RIVAL_DPI_4000 = 0x01,
	RIVAL_DPI_2000 = 0x02,
	RIVAL_DPI_1750 = 0x03,
	RIVAL_DPI_1500 = 0x04,
	RIVAL_DPI_1250 = 0x05,
	RIVAL_DPI_1000 = 0x06,
	RIVAL_DPI_500 = 0x07,
	RIVAL_DPI_250 = 0x08
} RIVAL_DPI;

typedef enum RIVAL_RATE {
	RIVAL_RATE_1000 = 0x01,
	RIVAL_RATE_500 = 0x02,
	RIVAL_RATE_250 = 0x03,
	RIVAL_RATE_125 = 0x04
} RIVAL_RATE;

typedef enum RIVAL_DPI_PRESET {
	RIVAL_DPI_PRESET_FIRST = 0x01,
	RIVAL_DPI_PRESET_SECOND = 0x02
} RIVAL_DPI_PRESET;

struct rival_device {
	int fd;
};

int rival_open_first(struct rival_device **dev);
void rival_close(struct rival_device *dev);

bool rival_get_name(struct rival_device *dev, uint8_t *buffer, size_t buffer_size);

bool rival_set_light_color(struct rival_device *dev, uint8_t r, uint8_t g, uint8_t b);
bool rival_set_light_mode(struct rival_device *dev, RIVAL_LIGHT_MODE mode);
bool rival_set_dpi(struct rival_device *dev, RIVAL_DPI_PRESET preset, RIVAL_DPI dpi);
bool rival_set_rate(struct rival_device *dev, RIVAL_RATE rate);
