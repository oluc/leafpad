/*
 *  leafpad.h
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

#ifndef _LEAFPAD_H
#define _LEAFPAD_H

/* autoconf */
#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

/* gettext */
#include "intl.h"

/* Shared Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

/* Original Functions */
#include "dialog.h"
#include "file.h"
#include "search.h"
#include "font.h"
#include "encoding.h"
#include "utils.h"
#include "keyevent.h"
#include "undo.h"
#include "indent.h"
#include "linenum.h"
#include "dnd.h"

/* Data Structure */
typedef struct {
	GtkWidget *window;
	GtkWidget *menubar;
	GtkWidget *textview;
	GtkTextBuffer *textbuffer;
} MainWindow;

typedef struct {
	MainWindow *mainwin;
	FileInfo *fi;
	gchar *filepath;
	/* config file */
	struct {
		gint width;
		gint height;
		gchar *fontname;
		gboolean wordwrap;
		gboolean linenumbers;
		gboolean autoindent;
		gchar *charset;
	} conf;
} StructData;

/* Local Functions */
#include "window.h"
#include "menu.h"
#include "callback.h"

#endif /* _LEAFPAD_H */
