#include <stdio.h>
#include "crawl.h"
#include "config.h"

int main()
{
    Config config = config_load("config.sconf");
    crawl("https://qtmsheep.com");

    config_free(config);

    return 0;
}
