#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "config.h"

bool rival_config_load(struct rival_config **config)
{
	struct rival_config tmp_config = {0};

	config_t inner_config = {0};
	config_init(&inner_config);

	if (!config_read_file(&inner_config, rival_config_filename)) {
		if (config_error_type(&inner_config) != CONFIG_ERR_FILE_IO) {
			return false;
		}

		// assume the config file doesn't exist yet and load the defaults
		rival_config_default(&tmp_config);
	} else {
		int dpi_preset1, dpi_preset2, rate, light_mode, r, g, b, enable_preview;

		if (!(config_lookup_int(&inner_config, "dpi_preset1", &dpi_preset1)
			&& config_lookup_int(&inner_config, "dpi_preset2", &dpi_preset2))
			&& config_lookup_int(&inner_config, "rate", &rate)
			&& config_lookup_int(&inner_config, "light_mode", &light_mode)
			&& config_lookup_int(&inner_config, "color_r", &r)
			&& config_lookup_int(&inner_config, "color_g", &g)
			&& config_lookup_int(&inner_config, "color_b", &b)
			&& config_lookup_bool(&inner_config, "enable_preview", &enable_preview)) {
			return false;
		}

		tmp_config.dpi_preset1 = dpi_preset1;
		tmp_config.dpi_preset2 = dpi_preset2;
		tmp_config.rate = rate;
		tmp_config.light_mode = light_mode;
		tmp_config.color_r = r;
		tmp_config.color_g = g;
		tmp_config.color_b = b;
		tmp_config.enable_preview = enable_preview;
	}

	*config = malloc(sizeof(struct rival_config));
	**config = tmp_config;
	return true;
}

bool rival_config_save(struct rival_config *config)
{
	config_t inner_config;
	config_init(&inner_config);
	config_setting_t *root = config_root_setting(&inner_config);

	config_setting_t *setting = config_setting_add(root, "dpi_preset1", CONFIG_TYPE_INT);
	config_setting_set_int(setting, config->dpi_preset1);
	setting = config_setting_add(root, "dpi_preset2", CONFIG_TYPE_INT);
	config_setting_set_int(setting, config->dpi_preset2);
	setting = config_setting_add(root, "rate", CONFIG_TYPE_INT);
	config_setting_set_int(setting, config->rate);
	setting = config_setting_add(root, "light_mode", CONFIG_TYPE_INT);
	config_setting_set_int(setting, config->light_mode);
	setting = config_setting_add(root, "color_r", CONFIG_TYPE_INT);
	config_setting_set_int(setting, config->color_r);
	setting = config_setting_add(root, "color_g", CONFIG_TYPE_INT);
	config_setting_set_int(setting, config->color_g);
	setting = config_setting_add(root, "color_b", CONFIG_TYPE_INT);
	config_setting_set_int(setting, config->color_b);
	setting = config_setting_add(root, "enable_preview", CONFIG_TYPE_BOOL);
	config_setting_set_bool(setting, config->enable_preview);

	return config_write_file(&inner_config, rival_config_filename);
}

void rival_config_default(struct rival_config *config)
{
	config->dpi_preset1 = RIVAL_DPI_1250;
	config->dpi_preset1 = RIVAL_DPI_2000;
	config->rate = RIVAL_RATE_1000;
	config->light_mode = RIVAL_LIGHT_MODE_STATIC;
	config->color_r = 0x00;
	config->color_g = 0x00;
	config->color_b = 0xFF;
	config->enable_preview = true;
}

void rival_config_free(struct rival_config *config)
{
	if (config) {
		free(config);
	}
}

static char *join_paths(char *p1, char *p2)
{
	size_t len = strlen(p1) + 1 + strlen(p2) + 1;
	char *dir = malloc(len);
	snprintf(dir, len, "%s/%s", p1, p2);
	return dir;
}

bool rival_config_prep()
{
	// chdir into $XDG_CONFIG_HOME/rival
	char *dir = NULL;
	char *dir_config = getenv("XDG_CONFIG_HOME");
	if (!dir_config) {
		// alright, $HOME/.config/rival it is
		char *dir_home = getenv("HOME");
		if (!dir_home) {
			// last resort
			struct passwd *entry = getpwuid(getuid());
			dir_home = entry->pw_dir;
		}
		dir = join_paths(dir_home, ".config/rival");
	} else {
		dir = join_paths(dir_config, "rival");
	}
	int res = chdir(dir);
	if (res != 0 && errno == ENOENT) {
		res = mkdir(dir, 0777);
	}
	if (res != 0) {
		printf("error: chdir or mkdir failed %d\n", errno);
		return false;
	}
	free(dir);
	return true;
}
