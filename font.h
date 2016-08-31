/*
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

#ifndef _FONT_H_
#define _FONT_H_ 1

/* Structures for storing bitmapped fonts.
 */

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
