#ifndef _ADSERV_CFG_H_
#define _ADSERV_CFG_H_

//#include <stdlib.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct {
	char *key;		/**< Requested key name */
	char *value;		/**< key value */
} cfg_req_t;

void adserv_cfg_free(void);
int adserv_cfg_load(char *path);
int adserv_cfg_get(cfg_req_t *req);

#endif /* _ADSERV_CFG_H_ */
