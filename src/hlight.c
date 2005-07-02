/*
 *  Leafpad - GTK+ based simple text editor
 *  Copyright (C) 2004-2005 Tarot Osuji
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

#include <string.h>
#include <gtk/gtk.h>

static gboolean searched_flag = FALSE;

static void cb_changed(GtkTextBuffer *buffer)
{
	GtkTextIter start, end;
	
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gtk_text_buffer_remove_tag_by_name(buffer,
		"searched", &start, &end);
	g_signal_handlers_block_by_func(G_OBJECT(buffer),
		G_CALLBACK(cb_changed), NULL);
	searched_flag = FALSE;
}

gboolean hlight_check_searched(void)
{
	return searched_flag;
}

gboolean hlight_toggle_searched(GtkTextBuffer *buffer)
{
	if (!searched_flag) {
		g_signal_handlers_unblock_by_func(G_OBJECT(buffer),
			G_CALLBACK(cb_changed), NULL);
		searched_flag = TRUE;
		return TRUE;
	}
	return FALSE;
}

void hlight_init(GtkTextBuffer *buffer)
{
	gtk_text_buffer_create_tag(buffer, "searched",
		"background", "yellow",
		"foreground", "black",
		NULL);
	gtk_text_buffer_create_tag(buffer, "replaced",
		"background", "cyan",
		"foreground", "black",
		NULL);
	g_signal_connect(G_OBJECT(buffer), "changed",
		G_CALLBACK(cb_changed), NULL);
	g_signal_handlers_block_by_func(G_OBJECT(buffer),
		G_CALLBACK(cb_changed), NULL);
}
