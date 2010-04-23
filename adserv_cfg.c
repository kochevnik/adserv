#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>

#include <adserv.h>
#include <adserv_cfg.h>

#define ADSERV_SECTION "main"

/**< configuration file name */
static char *cfg_file;

static GKeyFile	*server_config;

void adserv_cfg_free(void)
{
	g_free(cfg_file);
	cfg_file = NULL;
	if (server_config) {
		g_key_file_free(server_config);
		server_config = NULL;
	}
}

int adserv_cfg_load(char *path)
{
	dbg("using config '%s'\n", path);

	if (cfg_file || server_config)
		adserv_cfg_free();

	cfg_file = g_strdup(path);
	if (!cfg_file) {
		log("unable to allocate memory\n");
		return ADSERV_ERROR;
	}

	server_config = g_key_file_new();
	if (!server_config) {
		log("unable to allocate memory\n");
		adserv_cfg_free();
		return ADSERV_ERROR;
	}

	GError *err = NULL;
	if (!g_key_file_load_from_file(server_config, cfg_file, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &err)) {
		log("Could not read configuration file '%s': %s\n", cfg_file, err->message);
		g_error_free(err);
		adserv_cfg_free();
		return ADSERV_ERROR;
	}
	return ADSERV_OK;
}

int adserv_cfg_get(cfg_req_t *req)
{
	req->value = NULL;
	if (!g_key_file_has_group(server_config, ADSERV_SECTION)) {
		log("Configuration does not have section '%s'\n", ADSERV_SECTION);
		return ADSERV_ERROR;
	}
	if (!g_key_file_has_key(server_config, ADSERV_SECTION, req->key, NULL)) {
		log("Configuration does not have key '%s' in section '%s'\n", req->key, ADSERV_SECTION);
		return ADSERV_ERROR;
	}
	req->value = g_key_file_get_value(server_config, ADSERV_SECTION, req->key, NULL);
	if (!req->value) {
		log("Configuration does not have value for key '%s' in section '%s'\n", req->key, ADSERV_SECTION);
		return ADSERV_ERROR;
	}
	g_strstrip(req->value);
	dbg("key '%s', value '%s'\n", req->key, req->value);
	return ADSERV_OK;
}

