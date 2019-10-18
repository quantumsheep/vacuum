#ifndef CRAWL_H
#define CRAWL_H

#include "datatypes/vector.h"

Vector *crawl(const char *url, int max_depth, Vector *visited);

#endif
