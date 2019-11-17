#ifndef URL_H
#define URL_H

typedef struct url_t URL;
struct url_t
{
    char *protocol;
    char *host;
    char *path;
    char *page;
    char *query;

    char *fullpath;
};

URL *url_init();

URL *url_parse(const char *url);
char *url_query_encode(const char *query);

void url_free(URL *url);

#endif
