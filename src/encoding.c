/*
 *  encoding.c
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

#include <glib.h>
#include <string.h>
#include "intl.h"
#include "encoding.h"

/* Imported from gdit */
struct _CharsetInfo {
	gint index;
	gchar *charset;
	const gchar *desc;
};

enum {
//	USASCII = 1,
	UTF8 = 1,
	ISO88591,
/*	ISO88592,
	ISO88593,
	ISO88594,*/
	ISO88595,
/*	ISO88596,
	ISO88596E,
	ISO88596I,
	ISO88597,
	ISO88598,
	ISO88598E,
	ISO88598I,
	ISO88599,
	ISO885910, */
	KOI8R,
	GB2312,
	BIG5,
	ISO2022JP,
//	ISO2022JP2,
	EUCJP,
	SHIFTJIS,
	ISO2022KR,
	EUCKR,
//	OTHER,
	END
};

static CharsetInfo charsets[] = {
	{ 0, "DUMMY", N_("Current Locale") },
//	{ USASCII, "US-ASCII", N_("ANSI") },
	{ UTF8, "UTF-8", N_("Unicode") },
	{ ISO88591, "ISO-8859-1", N_("Western") },
/*	{ ISO88592, "ISO-8859-2", N_("Central European") },
	{ ISO88593, "ISO-8859-3", N_("South European") },
	{ ISO88594, "ISO-8859-4", N_("Baltic") }, */
	{ ISO88595, "ISO-8859-5", N_("Cyrillic") },
/*	{ ISO88596, "ISO-8859-6", N_("Arabic") },
	{ ISO88596, "ISO-8859-6-E", N_("Arabic") },
	{ ISO88596, "ISO-8859-6-I", N_("Arabic") },
	{ ISO88597, "ISO-8859-7", N_("Greek") },
	{ ISO88598, "ISO-8859-8", N_("Hebrew") },
	{ ISO88598E, "ISO-8859-8-E", N_("Hebrew") },
	{ ISO88598I, "ISO-8859-8-I", N_("Hebrew") },
	{ ISO88599, "ISO-8859-9", N_("Turkish") },
	{ ISO885910, "ISO-8859-10", N_("Nordic") }, */
	{ KOI8R, "KOI8-R", N_("Cyrillic") },
	{ GB2312, "GB2312", N_("Chinese Simplified") },
	{ BIG5, "Big5", N_("Chinese Traditional") },
	{ ISO2022JP, "ISO-2022-JP", N_("Japanese") },
//	{ ISO2022JP2, "ISO-2022-JP-2", N_("Japanese") },
	{ EUCJP, "EUC-JP", N_("Japanese") },
	{ SHIFTJIS, "Shift_JIS", N_("Japanese") },
	{ ISO2022KR, "ISO-2022-KR", N_("Korean") },
	{ EUCKR, "EUC-KR", N_("Korean") },
//	{ OTHER, "DUMMY", N_("Other Codeset") },
};

const gchar *get_default_charset(void)
{
	const gchar *charset;
	
	g_get_charset(&charset);
	if (g_strrstr(charset, "ANSI_X3.4"))
		return "US-ASCII";
	
	return charset;
}

void init_charset_info(void)
{
	charsets[0].charset = (gchar *) get_default_charset();
}

const CharsetInfo *get_charset_info_from_charset(const gchar *charset)
{
//	gint i = 1;
	gint i = 0;

	while (i < END) {
		if (charset == charsets[i].charset)
			return &charsets[i];
      
		++i;
	}
 
	return NULL;
}

const CharsetInfo *get_charset_info_from_index(gint index)
{
//	g_return_val_if_fail(index >= 1, NULL);
//	g_return_val_if_fail(index < END, NULL);
	
	if (index >= END)
		return NULL;
	
	return &charsets[index];
}

/* TODO: deprecate... */
gchar *get_string_from_charset_info(const CharsetInfo *ci)
{
	g_return_val_if_fail(ci != NULL, NULL);
	g_return_val_if_fail(ci->desc != NULL, NULL);
	g_return_val_if_fail(ci->charset != NULL, NULL);

	return g_strdup_printf("%s (%s)", ci->desc, ci->charset);
}

const gchar *get_charset_from_charset_info(const CharsetInfo* ci)
{
	g_return_val_if_fail(ci != NULL, NULL);
	g_return_val_if_fail(ci->charset != NULL, NULL);

	return ci->charset;
}

/*
Imported from KEdit (for BeOS, NOT KDE).
based on
http://examples.oreilly.com/cjkvinfo/Ch7/DetectCodeType.c
*/
void convert_line_ending_to_lf(gchar *text)
{
	gint i, j;
	
	for (i = 0, j = 0; TRUE; i++, j++) {
		if (*(text + i) == CR) {
			*(text + j) = LF;
			if (*(text + i + 1) == LF)
				i++;
		} else {
			*(text + j) = *(text + i);
			if (*(text + j) == '\0')
				break;
		}
	}
}

void convert_line_ending(gchar **text, gint retcode)
{
	gchar *buf, *str = *text;
	const gint len = strlen(str);
	gint i, j, LFNum = 0;
	
	switch (retcode) {
		
	case CR:
		while (*str != '0') {
			if (*str == LF)
				*str = CR;
			str++;
		}
		break;
		
	case CR+LF:
		for (i = 0; *(str + i) != '\0'; i++) {
			if (*(str + i) == LF)
				LFNum++;
		}
		buf = g_new(gchar, len + LFNum + 1);
		for (i= 0, j = 0;; i++, j++) {
			if (*(str + j) == LF) {
				*(buf + i) = CR;
				*(buf + (++i)) = LF;
			} else
				*(buf + i) = *(str + j);
			if (*(str + j) == '\0')
				break;
		}
		g_free(*text);
		*text = buf;
		
	}
}

gint detect_line_ending(const gchar *text)
{
	while (*(text++) != '\0') {
		if (*text == LF)
			break;
		if (*text == CR) {
			if (*(++text) == LF)
				return CR+LF;
			else
				return CR;
		}
	}
	return LF;
}

static gchar *detect_charset_ru(const gchar *text)
{
	guint8 c = *text;
	gchar *charset = "KOI8-R";
	gboolean noniso = FALSE;
	guint32 xc = 0, xd = 0, xef = 0;
	
	while ((c = *text++) != '\0') {
		if (c >= 0x80 && c <= 0x9F)
			noniso = TRUE;
		else if (c >= 0xC0 && c <= 0xCF)
			xc++;
		else if (c >= 0xD0 && c <= 0xDF)
			xd++;
		else if (c >= 0xE0)
			xef++;
	}
	
	if (!noniso && ((xc + xef) < xd))
		charset = "ISO-8859-5";
	else if ((xc + xd) < xef)
		charset = "windows-1251";
	
	return charset;
}

static gchar *detect_charset_zh(const gchar *text)
{
	guint8 c = *text;
	gchar *charset = "GB2312";
	
	while ((c = *text++) != '\0') {
		if (c >= 0x81 && c <= 0x87) {
			charset = "GB18030";
			break;
		}
		else if (c >= 0x88 && c <= 0xA0) {
			c = *text++;
			if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = "GB18030";
				break;
			} //else GBK/Big5-HKSCS cannot determine
		}
		else if ((c >= 0xA1 && c <= 0xC6) || (c >= 0xC9 && c <= 0xF9)) {
			c = *text++;
			if (c >= 0x40 && c <= 0x7E)
				charset = "Big5";
			else if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = "GB18030";
				break;
			}
		}
		else if (c >= 0xC7) {
			c = *text++;
			if ((c >= 0x30 && c <= 0x39) || (c >= 0x80 && c <= 0xA0)) {
				charset = "GB18030";
				break;
			}
		}
	}
	
	return charset;
}

static gchar *detect_charset_ja(const gchar *text)
{
	guint8 c = *text;
	gchar *charset = NULL;
	
	while (charset == NULL && (c = *text++) != '\0') {
		if (c >= 0x81 && c <= 0x9F) {
			if (c == 0x8E) /* SS2 */ {
				c = *text++;
				if ((c >= 0x40 && c <= 0xA0) || (c >= 0xE0 && c <= 0xFC))
					charset = "Shift_JIS";
			}
			else if (c == 0x8F) /* SS3 */ {
				c = *text++;
				if (c >= 0x40 && c <= 0xA0)
					charset = "Shift_JIS";
				else if (c >= 0xFD)
					break;
			}
			else
				charset = "Shift_JIS";
		}
		else if (c >= 0xA1 && c <= 0xDF) {
			c = *text++;
			if (c <= 0x9F)
				charset = "Shift_JIS";
			else if (c >= 0xFD)
				break;
		}
		else if (c >= 0xE0 && c <= 0xEF) {
			c = *text++;
			if (c >= 0x40 && c <= 0xA0)
				charset = "Shift_JIS";
			else if (c >= 0xFD)
				break;
		}
		else if (c >= 0xF0)
			break;
	}
	
	if (charset == NULL)
		charset = "EUC-JP";
	
	return charset;
}

static gboolean detect_noniso(const gchar *text)
{
	guint8 c = *text;
	
	while ((c = *text++) != '\0') {
		if (c >= 0x80 && c <= 0x9F)
			return TRUE;
	}
	return FALSE;
}

gchar *detect_charset(const gchar *text)
{
	guint8 c = *text;
	const gchar *env;
	gchar *charset = NULL;
	
	if (g_utf8_validate(text, -1, NULL)) {
		while ((c = *text++) != '\0') {
			if (c > 0x7F)
				return "UTF-8";
			if (c == 0x1B) /* ESC */ {
				c = *text++;
				if (c == '$') {
					c = *text++;
					switch (c) {
					case 'B': // JIS X 0208-1983
					case '@': // JIS X 0208-1978
						charset = "ISO-2022-JP";
						break;
					case 'A': // GB2312-1980
						charset = "ISO-2022-JP-2";
						break;
					case '(':
						c = *text++;
						switch (c) {
						case 'C': // KSC5601-1987
						case 'D': // JIS X 0212-1990
							charset = "ISO-2022-JP-2";
						}
						break;
					case ')':
						c = *text++;
						if (c == 'C')
							charset = "ISO-2022-KR"; // KSC5601-1987
					}
					if (strcmp(charset, "ISO-2022-JP-2") == 0 ||
						strcmp(charset, "ISO-2022-KR") == 0)
						break;
				}
			}
		}
		if (!charset) {
			charset = "US-ASCII";
		}
	}
	
	if (!charset) {
		env = g_getenv("LC_ALL");
/*		if (!env)
			env = g_getenv("LC_CTYPE");
*/		if (!env)
			env = g_getenv("LANG");
		
		if (env) {
			if (strncmp(env, "ru", 2) == 0)
				charset = detect_charset_ru(text);
			else if (strncmp(env, "zh", 2) == 0)
				charset = detect_charset_zh(text);
			else if (strncmp(env, "ja", 2) == 0)
				charset = detect_charset_ja(text);
			else if (strncmp(env, "ko", 2) == 0)
				charset = "EUC-KR";
			else {
				if (strncasecmp(get_default_charset(), "ISO", 3) == 0) {
					if (detect_noniso(text)) {
						charset = (gchar *) get_default_charset();
						if (g_strrstr(charset, "8859-2"))
							charset = "windows-1250";
						else if (g_strrstr(charset, "8859-3"))
							charset = NULL;
						else if (g_strrstr(charset, "8859-4"))
							charset = "windows-1257";
						else if (g_strrstr(charset, "8859-5"))
							charset = "windows-1251";
						else if (g_strrstr(charset, "8859-6"))
							charset = "windows-1256";
						else if (g_strrstr(charset, "8859-7"))
							charset = "windows-1253";
						else if (g_strrstr(charset, "8859-8"))
							charset = "windows-1255";
						else if (g_strrstr(charset, "8859-9"))
							charset = "windows-1254";
						else if (g_strrstr(charset, "8859-10"))
							charset = NULL; //? "windows-1257";
				//		else if (g_strrstr(charset, "8859-11"))
				//			charset = NULL;
				//		else if (g_strrstr(charset, "8859-12"))
				//			charset = NULL;
						else if (g_strrstr(charset, "8859-13"))
							charset = "windows-1257";
						else if (g_strrstr(charset, "8859-14"))
							charset = NULL; //? "windows-1254";
						else if (g_strrstr(charset, "8859-16"))
							charset = NULL;
						else
							charset = "windows-1252";
					}
				}
			}
		}
	}
	
	return charset;
}
