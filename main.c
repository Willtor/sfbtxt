#include "font.h"
#include <malloc.h>
#include <stdio.h>

typedef enum alignment_t alignment_t;

enum alignment_t { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER };

typedef struct config_t config_t;

struct config_t
{
    font_t *font;
    const char *output_file;
    FILE *input;
    alignment_t alignment;
};

typedef struct line_t line_t;

struct line_t
{
    line_t *next;
    const char *text;
    int width, height;
};

void process_args (config_t *config, int argc, char *argv[])
{
    config->output_file = "t.png";
    config->input = stdin;
    config->alignment = ALIGN_LEFT;
}

line_t *line_list_rev (line_t *list)
{
    line_t *iter = list;
    line_t *ret = NULL;
    while (iter) {
        line_t *tmp = iter->next;
        iter->next = ret;
        ret = iter;
        iter = tmp;
    }
    return ret;
}

line_t *read_text (FILE *fp)
{
    line_t *ret = NULL;

    while (!feof(fp)) {
        char *text = NULL;
        size_t sz = 0;
        line_t *tmp = NULL;
        if (-1 == getline(&text, &sz, fp)) {
            return line_list_rev(ret);
        }
        tmp = (line_t*)malloc(sizeof(line_t));
        tmp->text = text;
        tmp->next = ret;
        ret = tmp;
    }

    return line_list_rev(ret);
}

int main (int argc, char *argv[])
{
    config_t config;
    line_t *input;

    process_args(&config, argc, argv);

    input = read_text(config.input);
    while (input) {
        printf("%s\n", input->text);
        input = input->next;
    }

    return 0;
}
