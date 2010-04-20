#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#include <adserv.h>

static PGconn *conn;

static void reset_conn(void)
{
	PQfinish(conn);
	conn = NULL;
}

int adserv_db_connect(const char *conninfo)
{
	if (conn) {
		log("conn is already initialized\n");
		return ADSERV_ERROR;
	}

	conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK) {
		log("Connection to database failed: %s", PQerrorMessage(conn));
		reset_conn();
		return ADSERV_ERROR;
	}

	return ADSERV_OK;
}

void adserv_db_close(void)
{
	// TODO: if it is the only action here, rename reset_conn() to adserv_db_close() in this file
	reset_conn();
}

static char* adserv_sql_escape(adserv_buf_t *buf, size_t *escaped_length)
{
	unsigned to_size = 2*buf->size + 1;
	char *to = malloc(to_size);
	if (!to) {
		log("unable to allocate memory\n");
		*escaped_length = 0;
		return NULL;
	}

	int error = 0;
	*escaped_length = PQescapeStringConn(conn, to, buf->data, buf->size, &error);
	if (error) {
		log("PQescapeStringConn() failed\n");
		free(to);
		*escaped_length = 0;
		return NULL;
	}
	return to;
}

#define GET_TEMPLATE_QUERY "SELECT template FROM templates WHERE place_id='%s' AND inst_id='%s' AND iframe='%s'"
unsigned char* adserv_db_get_template(adserv_params_t *params, size_t *length)
{
	PGresult *res;
	char *buf, *escaped_place_id, *escaped_inst_id, *escaped_iframe;
	size_t buf_size, escaped_place_id_length, escaped_inst_id_length, escaped_iframe_length;

	*length = 0;

	escaped_place_id = adserv_sql_escape(&params->place_id, &escaped_place_id_length);
	if (!escaped_place_id)
		return NULL;

	escaped_inst_id = adserv_sql_escape(&params->inst_id, &escaped_inst_id_length);
	if (!escaped_inst_id) {
		free(escaped_place_id);
		return NULL;
	}

	escaped_iframe = adserv_sql_escape(&params->iframe, &escaped_iframe_length);
	if (!escaped_iframe) {
		free(escaped_place_id);
		free(escaped_inst_id);
		return NULL;
	}

	buf_size = sizeof(GET_TEMPLATE_QUERY) + escaped_place_id_length + escaped_inst_id_length + escaped_iframe_length;
	buf = malloc(buf_size);
	if (!buf) {
		log("unable to allocate memory");
		free(escaped_place_id);
		free(escaped_inst_id);
		free(escaped_iframe);
		return NULL;
	}
	snprintf(buf, buf_size, GET_TEMPLATE_QUERY, escaped_place_id, escaped_inst_id, escaped_iframe);
	free(escaped_place_id);
	free(escaped_inst_id);
	free(escaped_iframe);

	res = PQexec(conn, buf);
	free(buf);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		log("database error");
		PQclear(res);
		return NULL;
	}
	int fields_num = PQnfields(res);
	int tuples_num = PQntuples(res);
	if ((1 != fields_num) || (1 != tuples_num)) {
		log("unexpected db response");
		PQclear(res);
		return NULL;
	}
	const char *escaped = PQgetvalue(res, 0, 0);
	unsigned char *unescaped = PQunescapeBytea((const unsigned char*)escaped, length);
	PQclear(res);
	unsigned char *template = malloc(*length);
	if (template)
		memcpy(template, unescaped, *length);
	else {
		log("unable to allocate memory\n");
		*length = 0;
	}
	PQfreemem(unescaped);
	return template;
}

#define GET_BANNER_QUERY "SELECT banner FROM banners WHERE place_id='%s' AND banner_id='%s'"
unsigned char* adserv_db_get_banner(adserv_params_t *params, size_t *length)
{
	PGresult *res;
	char *buf, *escaped_place_id, *escaped_banner_id;
	size_t buf_size, escaped_place_id_length, escaped_banner_id_length;

	*length = 0;

	escaped_place_id = adserv_sql_escape(&params->place_id, &escaped_place_id_length);
	if (!escaped_place_id)
		return NULL;

	escaped_banner_id = adserv_sql_escape(&params->banner_id, &escaped_banner_id_length);
	if (!escaped_banner_id) {
		free(escaped_place_id);
		return NULL;
	}

	buf_size = sizeof(GET_BANNER_QUERY) + escaped_place_id_length + escaped_banner_id_length;
	buf = malloc(buf_size);
	if (!buf) {
		log("unable to allocate memory");
		free(escaped_place_id);
		free(escaped_banner_id);
		return NULL;
	}
	snprintf(buf, buf_size, GET_BANNER_QUERY, escaped_place_id, escaped_banner_id);
	free(escaped_place_id);
	free(escaped_banner_id);

	res = PQexec(conn, buf);
	free(buf);
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		log("database error");
		PQclear(res);
		return NULL;
	}
	int fields_num = PQnfields(res);
	int tuples_num = PQntuples(res);
	if ((1 != fields_num) || (1 != tuples_num)) {
		log("unexpected db response");
		PQclear(res);
		return NULL;
	}
	const char *escaped = PQgetvalue(res, 0, 0);
	unsigned char *unescaped = PQunescapeBytea((const unsigned char*)escaped, length);
	PQclear(res);
	unsigned char *banner = malloc(*length);
	if (banner)
		memcpy(banner, unescaped, *length);
	else {
		log("unable to allocate memory\n");
		*length = 0;
	}
	PQfreemem(unescaped);
	return banner;
}

