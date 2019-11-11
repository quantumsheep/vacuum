#ifndef CRAWL_H
#define CRAWL_H

#include "datatypes/vector.h"

typedef struct crawl_config_t CrawlConfig;
struct crawl_config_t
{
    int max_depth;
    int versioning;
};

Vector *crawl(const char *url, CrawlConfig config, Vector *visited);

#endif
