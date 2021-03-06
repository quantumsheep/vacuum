#include "config.h"
#include "../utils/file.h"

#include <stddef.h>
#include <string.h>

ConfigToken *config_create_token(ConfigTokenType type, char *value, int line, int copy_has_ref)
{
    ConfigToken *token = (ConfigToken *)malloc(sizeof(ConfigToken));

    token->type = type;
    token->line = line;

    if (copy_has_ref)
    {
        token->value = value;
    }
    else
    {
        token->value = (char *)calloc(sizeof(char), strlen(value) + 1);
        strcpy(token->value, value);
    }

    return token;
}

static int starts_with(const char *str, const char *pre)
{
    size_t len_str = strlen(str);
    size_t len_pre = strlen(pre);

    return len_str >= len_pre && memcmp(pre, str, len_pre) == 0;
}

Vector *config_tokenize(const char *path, int *has_error)
{
    Vector *tokens = vector_init();

    char *sources = file_read(path);
    size_t len = strlen(sources);

    int line = 1;

    for (size_t i = 0; i < len; i++)
    {
        char *cursor = sources + i;

        if (starts_with(cursor, "->"))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_OPTION_ARROW, "->", line, 0));
            i++;
        }
        else if (starts_with(cursor, "=="))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_TASK, "==", line, 0));
            i++;
        }
        else if (starts_with(cursor, "="))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_ACTION, "=", line, 0));
        }
        else if (starts_with(cursor, "+"))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_SEPARATOR, "+", line, 0));
        }
        else if (starts_with(cursor, "{"))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_OPENING_OPTION, "{", line, 0));
        }
        else if (starts_with(cursor, "}"))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_CLOSING_OPTION, "}", line, 0));
        }
        else if (starts_with(cursor, "("))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_OPENING_ARRAY, "(", line, 0));
        }
        else if (starts_with(cursor, ")"))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_CLOSING_ARRAY, ")", line, 0));
        }
        else if (starts_with(cursor, ","))
        {
            vector_push_ref(tokens, config_create_token(TOKEN_ARRAY_SEPARATOR, ",", line, 0));
        }
        else if (starts_with(cursor, "#"))
        {
            ptrdiff_t diff = strchr(cursor, '\n') - cursor;
            i += diff - 1;
        }
        else if ((*cursor >= 'a' && *cursor <= 'z') || (*cursor >= 'A' && *cursor <= 'Z') || (*cursor >= '0' && *cursor <= '9') || (*cursor == '_') || (*cursor == '-'))
        {
            int len = 0;

            do
            {
                len++;
            } while (cursor[len] != '\0' && !starts_with(cursor + len, "->") && cursor[len] != '}' && cursor[len] != ')' && cursor[len] != ',');

            /**
             * Trim string's end
             */
            while (len > 0 && cursor[len - 1] == ' ')
            {
                len--;
            }

            char *value = (char *)calloc(sizeof(char), len + 1);
            memcpy(value, cursor, len);

            vector_push_ref(tokens, config_create_token(TOKEN_STRING, value, line, 1));

            i += len - 1;
        }
        else if (*cursor == '\n')
        {
            line++;
        }
        else if (*cursor != ' ' && *cursor != '\r')
        {
            printf("Invalid token '%c' on line %d.\n", *cursor, line);
            *has_error = 1;
            break;
        }
    }

    return tokens;
}
