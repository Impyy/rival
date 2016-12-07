#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include "gui.h"
#include "rival.h"

static bool finished = false;
static struct rival_device *dev = NULL;

static void on_change_color(uint8_t r, uint8_t g, uint8_t b)
{
	rival_set_light_color(dev, r, g, b);
}

static void on_change_mode(RIVAL_LIGHT_MODE mode)
{
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

	gui_register_handler_color(&on_change_color);
	gui_register_handler_mode(&on_change_mode);
	gui_register_handler_dpi(&on_change_dpi);
	gui_register_handler_rate(&on_change_rate);
	gui_register_handler_close(&on_close);

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

	rival_close(dev);
	return 0;
}
