#include <assert.h>
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

extern font_t description;

/** Process the arguments and populate the config structure.
 */
void process_args (config_t *config, int argc, char *argv[])
{
    config->font = &description;
    config->output_file = "t.png";
    config->input = stdin;
    config->alignment = ALIGN_LEFT;
}

/** Reverse a linked list of lines.
 */
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

/** Read text from the input stream and return it as a list of line_t objects.
 */
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
        tmp->next = ret;
        tmp->text = text;
        tmp->width = 0;
        tmp->height = 0;

        ret = tmp;
    }

    return line_list_rev(ret);
}

/** Gather statistics on the input and store the required dimensions of the
 *  image in the out-parameters: width, height.  Also, fill in the individual
 *  line objects.
 */
void statistics (int *width, int *height, line_t *lines, font_t *font)
{
    *width = *height = 0;
    while (lines) {
        int i;
        for (i = 0; lines->text[i] != '\0'; ++i) {
            char c = lines->text[i];
            assert((int)c < 128);
            character_t *character = font->charset[(int)c];
            if (lines->height < character->height) {
                lines->height = character->height;
            }
            if (c != '\n') lines->width += character->width;
        }
        *height += lines->height;
        if (*width < lines->width) *width = lines->width;

        lines = lines->next;
    }
}

int main (int argc, char *argv[])
{
    config_t config;
    line_t *input;
    int width, height;

    process_args(&config, argc, argv);

    input = read_text(config.input);
    statistics(&width, &height, input, config.font);

    printf("png of %dx%d\n", width, height);

    return 0;
}
