#pragma once

#include <libconfig.h>

#include "rival.h"

static const char *rival_config_filename = "rival.conf";

struct rival_config {
	RIVAL_DPI dpi;
	RIVAL_RATE rate;
	RIVAL_LIGHT_MODE light_mode;
	uint8_t color_r;
	uint8_t color_g;
	uint8_t color_b;
	bool enable_preview;
};

bool rival_config_prep();
bool rival_config_load(struct rival_config **config);
bool rival_config_save(struct rival_config *config);
void rival_config_default(struct rival_config *config);
void rival_config_free(struct rival_config *config);
