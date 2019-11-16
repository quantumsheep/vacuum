#ifndef URL_H
#define URL_H

typedef struct url_t URL;
struct url_t
{
    char *host;
    char *path;
    char *page;
    char *query;

    char *fullpath;
};

URL *url_init();

URL *url_parse(const char *url);

void url_free(URL *url);

#endif
