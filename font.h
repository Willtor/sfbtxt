#ifndef _FONT_H_
#define _FONT_H_ 1

typedef struct character_t character_t;

struct character_t
{
    int width, height;
    unsigned long long *bitmap; // Max width of 64 pixels.
};

typedef struct font_t font_t;

struct font_t
{
    char *name;
    character_t **charset;
};

#endif // !defined _FONT_H_
