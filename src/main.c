#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include "config.h"
#include "gui.h"
#include "rival.h"

static bool finished = false;
static struct rival_device *dev = NULL;
static struct rival_config *config;

static void on_change_color(uint8_t r, uint8_t g, uint8_t b)
{
	// don't set the color if mode is equal to RIVAL_LIGHT_MODE_OFF
	// as this will turn the light back on
	if (config->light_mode == RIVAL_LIGHT_MODE_OFF) {
		return;
	}

	rival_set_light_color(dev, r, g, b);
}

static void on_change_mode(RIVAL_LIGHT_MODE mode)
{
	// set the light color just in case the previous mode was RIVAL_LIGHT_MODE_OFF
	if (mode != RIVAL_LIGHT_MODE_OFF) {
		rival_set_light_color(dev, config->color_r, config->color_g, config->color_b);
	}

	rival_set_light_mode(dev, mode);
}

static void on_change_dpi(RIVAL_DPI_PRESET preset, RIVAL_DPI dpi)
{
	rival_set_dpi(dev, preset, dpi);
}

static void on_change_rate(RIVAL_RATE rate)
{
	rival_set_rate(dev, rate);
}

static void on_apply()
{
	rival_set_light_color(dev, config->color_r, config->color_g, config->color_b);
	rival_set_light_mode(dev, config->light_mode);
	rival_set_dpi(dev, RIVAL_DPI_PRESET1, config->dpi_preset1);
	rival_set_dpi(dev, RIVAL_DPI_PRESET2, config->dpi_preset2);
	rival_set_rate(dev, config->rate);
}

static void on_save()
{
	on_apply();

	rival_save(dev);
	rival_config_save(config);
}

static void on_close()
{
	finished = true;
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

	int error = rival_open_first(&dev);
	if (error != 0) {
		printf("unable to initialize librival: %d\n", error);
		return 1;
	}

	char dev_name[256] = {0};
	if (rival_get_name(dev, dev_name, sizeof(dev_name))) {
		gui_queue((void (*)(void *))gui_set_title, dev_name);
	}

	while (!finished) {
		struct timespec ts = {0};
		ts.tv_nsec = 100000000L;
		nanosleep(&ts, NULL);
	}

	rival_config_free(config);
	rival_close(dev);
	return 0;
}
