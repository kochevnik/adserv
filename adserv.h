#ifndef ADSERV_H
#define ADSERV_H

#define ADSERV_OK	0
#define ADSERV_ERROR	1

#include <stdio.h>
#define log(format, ...) do {printf("%s() ", __FUNCTION__); printf(format, ## __VA_ARGS__); } while (0)
#define dbg(format, ...) do {printf("%s() ", __FUNCTION__); printf(format, ## __VA_ARGS__); } while (0)

typedef struct {
	size_t size;
	char *data;
} adserv_buf_t;

typedef struct {
	adserv_buf_t place_id;
	adserv_buf_t inst_id;
	adserv_buf_t iframe;
	adserv_buf_t banner_id;
} adserv_params_t;

#ifdef offset_of
#undef offset_of
#endif
#define offset_of(type,field) ((unsigned)((char*)&((type*)0UL)->field - (char*)0UL))

#define adserv_params_offset(field) (offset_of(adserv_params_t, field))

#define CONTENT_TYPE_IMAGE	0
#define CONTENT_TYPE_FLASH	1

#define EVENT_TYPE_GET_TEMPLATE	2
#define EVENT_TYPE_GET_REDIRECT	3
#define EVENT_TYPE_GET_FILE	4

#define LINK_OPEN_TYPE_BLANK	0
#define LINK_OPEN_TYPE_SELF	1

#endif // ADSERV_H
