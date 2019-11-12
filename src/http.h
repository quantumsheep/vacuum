#ifndef HTTP_H
#define HTTP_H

#include "datatypes/map.h"

typedef struct http_response_t HttpResponse;
struct http_response_t
{
    int status;
    char *content_type;
    char *buffer;
};

HttpResponse http_get(const char *url);

#endif
