#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/queue.h>
#include <evhttp.h>
#include <libmemcached/memcached.h>

#include <adserv.h>
#include <adserv_db.h>
#include <adserv_cfg.h>

memcached_st *cache_client;

#define ADSERV_GET_TEMPLATE_KEY "template:%s:%s:%s"
static char* get_template(adserv_params_t *params, size_t *reply_size)
{
	char *reply, *key;
	size_t key_size;
	memcached_return rc;
	uint32_t flags;

	if (!(params->place_id.data && params->inst_id.data)) {
		log("bad request\n");
		return NULL;
	}

	if (!params->iframe.data) {
		params->iframe.data = "false";
		params->iframe.size = sizeof("false");
	}

	key_size = sizeof(ADSERV_GET_TEMPLATE_KEY) + params->place_id.size + params->inst_id.size + params->iframe.size;
	dbg("key_size %lu\n", (unsigned long)key_size);
	key = malloc(key_size);
	if (!key) {
		log("unable to allocate memory\n");
		*reply_size = 0;
		return NULL;
	}
	key_size = snprintf(key, key_size, ADSERV_GET_TEMPLATE_KEY, params->place_id.data, params->inst_id.data, params->iframe.data);

	flags = 0;
	reply = memcached_get(cache_client, key, key_size, reply_size, &flags, &rc);
	if (reply) {
		dbg("cached, reply_size = %lu\n", (unsigned long)*reply_size);
	} else {
		dbg("not cached\n");
		reply = (char*)adserv_db_get_template(params, reply_size);
		if (reply) {
			dbg("reply_size = %lu\n", (unsigned long)*reply_size);
			memcached_set(cache_client, key, key_size, reply, *reply_size, 300, 0);
		}
	}
	free(key);
	return reply;
}

#define ADSERV_GET_BANNER_KEY "banner:%s:%s"
static char* get_banner(adserv_params_t *params, size_t *reply_size)
{
	char *reply, *key;
	size_t key_size;
	memcached_return rc;
	uint32_t flags;

	if (!(params->place_id.data && params->banner_id.data)) {
		log("bad request\n");
		return NULL;
	}

	key_size = sizeof(ADSERV_GET_BANNER_KEY) + params->place_id.size + params->banner_id.size;
	dbg("key_size %lu\n", (unsigned long)key_size);
	key = malloc(key_size);
	if (!key) {
		log("unable to allocate memory\n");
		*reply_size = 0;
		return NULL;
	}
	key_size = snprintf(key, key_size, ADSERV_GET_BANNER_KEY, params->place_id.data, params->banner_id.data);

	flags = 0;
	reply = memcached_get(cache_client, key, key_size, reply_size, &flags, &rc);
	if (reply) {
		dbg("cached, reply_size = %lu\n", (unsigned long)*reply_size);
	} else {
		dbg("not cached\n");
		reply = (char*)adserv_db_get_banner(params, reply_size);
		if (reply) {
			dbg("reply_size = %lu\n", (unsigned long)*reply_size);
			memcached_set(cache_client, key, key_size, reply, *reply_size, 300, 0);
		}
	}
	free(key);
	return reply;
}

static void handler(struct evhttp_request *req, void *arg)
{
	struct evbuffer *buf;
	unsigned event_id;
	adserv_params_t params;
	struct evkeyval *itr;
	struct evkeyvalq query_params;

	dbg("req->uri = '%s'\n", req->uri);

	event_id = 0;
	evhttp_parse_query(req->uri, &query_params);
	TAILQ_FOREACH(itr, &query_params, next) {
		dbg("param key = '%s', val = '%s'\n", itr->key, itr->value);
		if (strcmp("eventId", itr->key) == 0)
			event_id = atoi(itr->value);
		if (strcmp("placeId", itr->key) == 0) {
			params.place_id.data = itr->value; // warning: storage lifetime!
			params.place_id.size = strlen(itr->value) + 1;
		}
		if (strcmp("instId", itr->key) == 0) {
			params.inst_id.data = itr->value; // warning: storage lifetime!
			params.inst_id.size = strlen(itr->value) + 1;
		}
		if (strcmp("iframe", itr->key) == 0) {
			params.iframe.data = itr->value; // warning: storage lifetime!
			params.iframe.size = strlen(itr->value) + 1;
		}
		if (strcmp("bannerId", itr->key) == 0) {
			params.banner_id.data = itr->value; // warning: storage lifetime!
			params.banner_id.size = strlen(itr->value) + 1;
		}
	}

	char *reply = NULL;
	size_t reply_size = 0;
	switch (event_id) {
	case 2:
		reply = get_template(&params, &reply_size);
		break;
	case 3:
		reply = get_banner(&params, &reply_size);
		break;
	default:
		log("unexpected eventId");
		evhttp_send_error(req, HTTP_BADREQUEST, "unexpected eventId");
		return;
	}

	if (!reply) {
		evhttp_send_error(req, HTTP_SERVUNAVAIL, "failed to get reply");
		return;
	}

	buf = evbuffer_new();
	if (buf) {
		evbuffer_add(buf, reply, reply_size);
		evhttp_send_reply(req, HTTP_OK, "OK", buf);
		evbuffer_free(buf);
	} else {
		evhttp_send_error(req, HTTP_SERVUNAVAIL, "failed to create response buffer");
	}

	free(reply);
}

#define DEFAULT_CONFIG "/etc/adserv.conf"
#define OPTSTRING "c:hid:"

static void usage(const char *arg)
{
	printf("Usage: %s [options]\n"
		"Options are:\n"
		" -c <config>\n\tPath to the main config file, default is '%s'\n"
		" -h\n\tPrint this help and exit\n"
		" -i\n\tDo not become daemon\n"
		" -d <debug_options>\n\tParameters for debug output\n"
		, arg, DEFAULT_CONFIG);
}

int main(int argc, char **argv)
{
	int opt, exit_status = EXIT_FAILURE;
	char *config = NULL;
	unsigned daemonize = 1;
	char *debug_str = NULL;

	while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
		switch (opt) {
		case 'c':
			config = strdup(optarg);
			break;
		case 'h':
			usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 'i':
			daemonize = 0;
			break;
		case 'd':
			debug_str = strdup(optarg);
			break;
		default:
			fprintf(stderr, "unknown option '%c'\n", opt);
			usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (!config)
		config = strdup(DEFAULT_CONFIG);
	if (adserv_cfg_load(config)) {
		free(config);
		exit(EXIT_FAILURE);
	}
	free(config);
	config = NULL;

	//if (daemonize)
	//	daemon(0, 0);

	cfg_req_t req;
	req.key = "db_conn_info";
	if (adserv_cfg_get(&req) != ADSERV_OK)
		goto out_cfg;
	if (adserv_db_connect(req.value) != ADSERV_OK)
		goto out_cfg;

	cache_client = memcached_create(NULL);
	if (!cache_client) {
		log("memcached_create() failed\n");
		goto out_db;
	}

	req.key = "memcached_host";
	if (adserv_cfg_get(&req) != ADSERV_OK)
		goto out_db;
	const char *memcached_host = req.value;

	req.key = "memcached_port";
	if (adserv_cfg_get(&req) != ADSERV_OK)
		goto out_db;
	unsigned memcached_port = atoi(req.value);

	if (memcached_server_add(cache_client, memcached_host, memcached_port) != MEMCACHED_SUCCESS) {
		log("unable to connect to memcached at %s:%u\n", memcached_host, memcached_port);
		goto out_db;
	}

	req.key = "listen_address";
	if (adserv_cfg_get(&req) != ADSERV_OK)
		goto out_db;
	const char *listen_address = req.value;

	req.key = "listen_port";
	if (adserv_cfg_get(&req) != ADSERV_OK)
		goto out_db;
	unsigned listen_port = atoi(req.value);

	event_init();
	struct evhttp *httpd = evhttp_start(listen_address, listen_port);
	if (!httpd) {
		log("evhttp_start() failed: %s\n", strerror(errno));
		adserv_db_close();
		exit(EXIT_FAILURE);
	}

	evhttp_set_gencb(httpd, handler, NULL);

	event_dispatch();

	evhttp_free(httpd);
	exit_status = EXIT_SUCCESS;
out_db:
	adserv_db_close();
out_cfg:
	adserv_cfg_free();
	exit(exit_status);
}

