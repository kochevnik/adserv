#ifndef ADSERV_DB_H
#define ADSERV_DB_H

int adserv_db_connect(const char *conninfo);
void adserv_db_close(void);
unsigned char* adserv_db_get_template(adserv_params_t *params, size_t *length);
unsigned char* adserv_db_get_banner(adserv_params_t *params, size_t *length);

#endif // ADSERV_DB_H
