#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include "config.h"
#include "gui.h"
#include "rival.h"

static bool end = false;
static struct rival *rival = NULL;
static struct rival_config *config;

static void on_change_color(uint8_t r, uint8_t g, uint8_t b)
{
	// don't set the color if mode is equal to RIVAL_LIGHT_MODE_OFF
	// as this will turn the light back on
	if (config->light_mode == RIVAL_LIGHT_MODE_OFF) {
		return;
	}

	rival_set_light_color(rival, r, g, b);
}

static void on_change_mode(RIVAL_LIGHT_MODE mode)
{
	// set the light color just in case the previous mode was RIVAL_LIGHT_MODE_OFF
	if (mode != RIVAL_LIGHT_MODE_OFF) {
		rival_set_light_color(rival, config->color_r, config->color_g, config->color_b);
	}

	rival_set_light_mode(rival, mode);
}

static void on_change_dpi(RIVAL_DPI_PRESET preset, RIVAL_DPI dpi)
{
	rival_set_dpi(rival, preset, dpi);
}

static void on_change_rate(RIVAL_RATE rate)
{
	rival_set_rate(rival, rate);
}

static void on_apply()
{
	rival_set_light_color(rival, config->color_r, config->color_g, config->color_b);
	rival_set_light_mode(rival, config->light_mode);
	rival_set_dpi(rival, RIVAL_DPI_PRESET1, config->dpi_preset1);
	rival_set_dpi(rival, RIVAL_DPI_PRESET2, config->dpi_preset2);
	rival_set_rate(rival, config->rate);
}

static void on_save()
{
	on_apply();

	rival_save(rival);
	rival_config_save(config);
}

static void on_close()
{
	end = true;
}

static void on_rival_opened()
{
	static const size_t size = 256;
	char *name = calloc(1, size);
	if (rival_get_name(rival, name, size)) {
		gui_queue((void (*)(void *))gui_set_title_heap, name);
	}
	gui_on_rival_opened();
}

static void on_rival_closed()
{
	gui_queue((void (*)(void *))gui_set_title, "rival");
	gui_on_rival_closed();
}

static void sig_handler(int sig)
{
	on_close();
}

int main()
{
	signal(SIGINT, sig_handler);

	if (!rival_config_prep()) {
		printf("unable to prepare config directory\n");
		return 1;
	}
	if (!rival_config_load(&config)) {
		printf("unable to load config\n");
		return 1;
	}

	struct gui_model model = {
		.handler_color = &on_change_color,
		.handler_mode = &on_change_mode,
		.handler_dpi = &on_change_dpi,
		.handler_rate = &on_change_rate,
		.handler_save = &on_save,
		.handler_apply = &on_apply,
		.handler_close = &on_close,

		.dpi_preset1 = &config->dpi_preset1,
		.dpi_preset2 = &config->dpi_preset2,
		.rate = &config->rate,
		.light_mode = &config->light_mode,
		.color_r = &config->color_r,
		.color_g = &config->color_g,
		.color_b = &config->color_b,
		.enable_preview = &config->enable_preview
	};
	gui_set_model(&model);

	if (!gui_setup()) {
		return 1;
	}

	pthread_t gui_thread;
	pthread_create(&gui_thread, NULL, &gui_main, NULL);

	rival = rival_new(on_rival_opened, on_rival_closed);
	rival_monitor(rival, &end);
	rival_config_free(config);
	rival_free(rival);
	return 0;
}
