/*
 *  encoding.h
 *  This file is part of Leafpad
 *
 *  Copyright (C) 2004 Tarot Osuji
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _ENCODING_H
#define _ENCODING_H

typedef struct {
	gint index;
	gchar *charset;
	gchar *desc;
} CharsetInfo;

const gchar *get_default_charset(void);
void init_charset_info(void);
const CharsetInfo *get_charset_info_from_charset(const gchar *charset);
const CharsetInfo *get_charset_info_from_index(gint index);
gchar *get_string_from_charset_info(const CharsetInfo *ci);
const gchar *get_charset_from_charset_info(const CharsetInfo* ci);

gint detect_line_ending(const gchar *text);
void convert_line_ending_to_lf(gchar *text);
void convert_line_ending(gchar **text, gint retcode);
gchar *detect_charset(const gchar *text);

enum {
	LF = 0x0A,
	CR = 0x0D,
};

#endif  /* _ENCODING_H */
