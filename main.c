/*

  SFBTXT: A program to generate custom graphics of text in the style of a
  Star Fleet Battles (copyright Amarillo Design Bureau) SSD.

Copyright (C) 2016  SFBTXT Authors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

 */

#include <assert.h>
#include "font.h"
#include <malloc.h>
#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    char *text;
    int width, height;
};

extern font_t description;

/** Print a usage message and exit.
 */
void usage ()
{
    fprintf(stderr, "usage: sfbtxt [options] <outfile.png>\n");
    fprintf(stderr, "  options\n");
    fprintf(stderr,
            "  -i <infile.txt>: Source of text.  If no file is specified,\n"
            "                   text is received from stdin.\n");
    exit(0);
}

/** Process the arguments and populate the config structure.
 */
void process_args (config_t *config, int argc, char *argv[])
{
    int i;

    // Defaults.
    config->font = &description;
    config->output_file = NULL; // User must specify an output file.
    config->input = stdin;
    config->alignment = ALIGN_LEFT;

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            // Switch.
            if (0 == strcmp(argv[i], "--help")
                || 0 == strcmp(argv[i], "-help")) {
                usage();
            } else if (0 == strcmp(argv[i], "-i")) {
                ++i;
                if (i >= argc || argv[i][0] == '-') {
                    fprintf(stderr,
                            "The -i flag requires an argument.\n"
                            "Use --help for options.\n");
                    exit(1);
                }
                config->input = fopen(argv[i], "r");
                if (NULL == config->input) {
                    fprintf(stderr, "Unable to open the input file: %s\n",
                            argv[i]);
                    exit(1);
                }
            } else {
                fprintf(stderr,
                        "Unrecognized option: %s.\n"
                        "Use --help for options.\n",
                        argv[i]);
                exit(1);
            }
        } else {
            // Output file.
            if (NULL != config->output_file) {
                fprintf(stderr,
                        "Specified multiple output files:\n\"%s\", \"%s\"\n"
                        "Use --help for options.\n",
                        config->output_file, argv[i]);
                exit(1);
            }
            config->output_file = argv[i];
        }
    }

    if (NULL == config->output_file) {
        fprintf(stderr, "Need an output file.  Use --help for options.\n");
        exit(1);
    }
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
            if (fp != stdin) fclose(fp);
            return line_list_rev(ret);
        }
        tmp = (line_t*)malloc(sizeof(line_t));
        tmp->next = ret;
        tmp->text = text;
        tmp->width = 0;
        tmp->height = 0;

        ret = tmp;
    }

    if (fp != stdin) fclose(fp);
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
            else lines->text[i] = '\0';
        }
        *height += lines->height;
        if (*width < lines->width) *width = lines->width;

        lines = lines->next;
    }
}

/** Paint a set of pixels given by the "pixels" bitmap to the current row.
 */
void paint_pixels (unsigned long long pixels, png_byte *row, int width)
{
    for ( ; width > 0; --width) {
        int on = ((pixels >> (width - 1)) & 0x1ULL) == 1;
        unsigned char byte = on ? (unsigned char)0 : (unsigned char)255;
        row[0] = byte;
        row[1] = byte;
        row[2] = byte;
        row[3] = (unsigned char)255;
        row += 4;
    }
}

/** Build the bitmap and write it to the output fp.  Return 0 on
 *  success, nonzero otherwise.
 */
int generate_png (FILE *fp, int width, int height, line_t *lines,
                  font_t *font)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_byte **rows = NULL;
    int x, y;
    line_t *curr;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                      NULL, NULL, NULL);
    if (NULL == png_ptr) return 1;
    info_ptr = png_create_info_struct(png_ptr);
    if (NULL == info_ptr) return 1;
    if (setjmp(png_jmpbuf(png_ptr))) return 1;

    png_set_IHDR(png_ptr, info_ptr, width, height, 8,
                 PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    rows = (png_byte**)png_malloc(png_ptr, height * sizeof(png_byte*));
    y = 0;
    curr = lines;
    while (curr) {
        int offset_y;
        for (offset_y = 0; offset_y < curr->height; ++offset_y, ++y) {
            int i;
            rows[y] = (png_byte*)png_malloc(png_ptr, width * 4);
            x = 0;
            for (i = 0; curr->text[i] != '\0'; ++i) {
                char c = curr->text[i];
                character_t *cbmp = font->charset[(int)c];
                unsigned long long pixels =
                    cbmp->height <= offset_y ?
                    0x0ULL : cbmp->bitmap[offset_y];
                paint_pixels(pixels, &rows[y][4 * x], cbmp->width);
                x += cbmp->width;
            }
            for ( ; x < width; ++x) {
                paint_pixels(0x0ULL, &rows[y][4 * x], 1);
            }
        }
        curr = curr->next;
    }

    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, rows);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    for (y = 0; y < height; ++y) png_free(png_ptr, rows[y]);
    png_free(png_ptr, rows);

    return 0;
}

/** Free the list of lines.
 */
void cleanup_lines (line_t *list)
{
    while (list) {
        line_t *tmp = list->next;
        free(list->text);
        free(list);
        list = tmp;
    }
}

int main (int argc, char *argv[])
{
    config_t config;
    line_t *input;
    int width, height;
    FILE *output;

    process_args(&config, argc, argv);

    input = read_text(config.input);
    statistics(&width, &height, input, config.font);

    output = fopen(config.output_file, "wb");
    if (NULL == output) {
        fprintf(stderr, "Unable to write to file: %s\n", config.output_file);
        return 1;
    }
    if (0 != generate_png(output, width, height, input, config.font)) {
        fprintf(stderr, "Failed to generate the image.\n");
        return 1;
    }
    fclose(output);
    cleanup_lines(input);

    return 0;
}
