/*
 *  indent.c
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

#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "undo.h"
#include "indent.h"

static gboolean auto_indent = FALSE;
static gint current_tab_width = 8;

void indent_change_state(gboolean state)
{
	auto_indent = state;
}

static gchar *compute_indentation(GtkTextBuffer *buffer, gint line) // from gedit
{
	GtkTextIter start_iter, end_iter;
	gunichar ch;
	
	gtk_text_buffer_get_iter_at_line(buffer, &start_iter, line);
	end_iter = start_iter;
	ch = gtk_text_iter_get_char(&end_iter);
	while (g_unichar_isspace(ch) && ch != '\n') {
		if (!gtk_text_iter_forward_char(&end_iter))
			break;
		ch = gtk_text_iter_get_char(&end_iter);
	}
	if (gtk_text_iter_equal(&start_iter, &end_iter))
		return NULL;
	
	return gtk_text_iter_get_text(&start_iter, &end_iter);
}

static void indent_real(GtkWidget *text_view)
{
	GtkTextIter iter;
	gchar *ind, *str;
	
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	
	gtk_text_buffer_delete_selection(buffer, TRUE, TRUE);
	gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
	ind = compute_indentation(buffer, gtk_text_iter_get_line(&iter));
	str = g_strconcat("\n", ind, NULL);
	gtk_text_buffer_insert(buffer, &iter, str, -1);
	g_free(str);
	g_free(ind);
	
	gtk_text_view_scroll_mark_onscreen(
		GTK_TEXT_VIEW(text_view),
		gtk_text_buffer_get_insert(buffer));
}

static gint calculate_real_tab_width(GtkWidget *text_view, guint tab_size) //from gtesourceview
{
	PangoLayout *layout;
	gchar *tab_string;
	gint tab_width = 0;

	if (tab_size == 0)
		return -1;

	tab_string = g_strnfill(tab_size, 0x20);
	layout = gtk_widget_create_pango_layout(text_view, tab_string);
	g_free (tab_string);

	if (layout != NULL) {
		pango_layout_get_pixel_size(layout, &tab_width, NULL);
		g_object_unref(G_OBJECT(layout));
	} else
		tab_width = -1;

	return tab_width;
}

void indent_refresh_tab_width(GtkWidget *text_view)
{
	PangoTabArray *tab_array;
	
	tab_array = pango_tab_array_new(1, TRUE);
	pango_tab_array_set_tab(tab_array, 0, PANGO_TAB_LEFT,
		calculate_real_tab_width(text_view, current_tab_width));
	gtk_text_view_set_tabs(GTK_TEXT_VIEW(text_view), tab_array);
	pango_tab_array_free(tab_array);
}

static void toggle_tab_width(GtkWidget *text_view)
{
//	PangoTabArray *tab_array;
	gint width = 8;
	
	if (current_tab_width == 8)
		width = 4;
	current_tab_width = width;
	indent_refresh_tab_width(text_view);
/*	tab_array = pango_tab_array_new(1, TRUE);
	pango_tab_array_set_tab(tab_array, 0, PANGO_TAB_LEFT,
		calculate_real_tab_width(text_view, width));
	gtk_text_view_set_tabs(GTK_TEXT_VIEW(text_view), tab_array);
	pango_tab_array_free(tab_array); */
}

static void multi_line_indent(GtkTextBuffer *buffer)
{
	GtkTextIter start_iter, end_iter, iter;
	gint start_line, end_line, i;
	gboolean pos;
	
	gtk_text_buffer_get_selection_bounds(buffer, &start_iter, &end_iter);
	start_line = gtk_text_iter_get_line(&start_iter);
	end_line = gtk_text_iter_get_line(&end_iter);
	gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
	pos = gtk_text_iter_equal(&iter, &start_iter);
	for (i = start_line; i < end_line; i++) {
		gtk_text_buffer_get_iter_at_line(buffer, &iter, i);
		gtk_text_buffer_place_cursor(buffer, &iter);
		gtk_text_buffer_insert(buffer, &iter, "\t", 1);
		undo_set_sequency(TRUE);
	}
	undo_set_sequency(FALSE);
	
	gtk_text_buffer_get_iter_at_line(buffer, &start_iter, start_line);
	gtk_text_buffer_get_iter_at_line(buffer, &end_iter, end_line);
	if (pos) {
		gtk_text_buffer_place_cursor(buffer, &end_iter);
		gtk_text_buffer_move_mark_by_name(buffer, "insert", &start_iter);
	} else {
		gtk_text_buffer_place_cursor(buffer, &start_iter);
		gtk_text_buffer_move_mark_by_name(buffer, "insert", &end_iter);
	}
}

static gint compute_indent_offset_length(const gchar *ind)
{
	guint8 c = *ind++;
	gint len = 1;
	
	while ((len < current_tab_width) && (c = *ind++) == 0x20) {
		len++;
	}
	
	return len;
}

static void multi_line_unindent(GtkTextBuffer *buffer)
{
	GtkTextIter start_iter, end_iter, iter;
	gint start_line, end_line, i, len;
	gboolean pos;
	gchar *ind;
	
	gtk_text_buffer_get_selection_bounds(buffer, &start_iter, &end_iter);
	start_line = gtk_text_iter_get_line(&start_iter);
	end_line = gtk_text_iter_get_line(&end_iter);
	gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));
	pos = gtk_text_iter_equal(&iter, &start_iter);
//	for (i = start_line; i < end_line; i++) {
	i = start_line;
	do {
		ind = compute_indentation(buffer, i);
		if (ind && strlen(ind)) {
			len = compute_indent_offset_length(ind);
			gtk_text_buffer_get_iter_at_line(buffer, &start_iter, i);
			gtk_text_buffer_place_cursor(buffer, &start_iter);
			end_iter = start_iter;
			gtk_text_iter_forward_chars(&end_iter, len);
			gtk_text_buffer_move_mark_by_name(buffer, "insert", &end_iter);
			gtk_text_buffer_delete(buffer, &start_iter, &end_iter);
			undo_set_sequency(TRUE);
			g_free(ind);
		}
		i++;
	} while (i < end_line);
//	}
	undo_set_sequency(FALSE);
	
	gtk_text_buffer_get_iter_at_line(buffer, &start_iter, start_line);
	gtk_text_buffer_get_iter_at_line(buffer, &end_iter, end_line);
	if (pos) {
		gtk_text_buffer_place_cursor(buffer, &end_iter);
		gtk_text_buffer_move_mark_by_name(buffer, "insert", &start_iter);
	} else {
		gtk_text_buffer_place_cursor(buffer, &start_iter);
		gtk_text_buffer_move_mark_by_name(buffer, "insert", &end_iter);
	}
}

static gboolean check_preedit(GtkWidget *text_view)
{
	gchar *str;
	
	gtk_im_context_get_preedit_string(
		GTK_TEXT_VIEW(text_view)->im_context, &str, NULL, NULL);
	if (strlen(str)) {
		g_free(str);
		return TRUE;
	}
	g_free(str);
	return FALSE;
}

static gboolean check_selection_bound(GtkTextBuffer *buffer)
{
	GtkTextIter start_iter, end_iter;
	gchar *str;
	
	if (gtk_text_buffer_get_selection_bounds(buffer, &start_iter, &end_iter)) {
		str = gtk_text_iter_get_text(&start_iter, &end_iter);
		if (g_strrstr(str, "\n")) {
			g_free(str);
			return TRUE;
		}
		g_free(str);
	}
	return FALSE;
}

static gboolean cb_key_press_event(GtkWidget *text_view, GdkEventKey *event)
{
	GtkTextBuffer *buffer =
		gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	
	if (event->keyval) keyval = event->keyval; // for undo.c (this line only!)
	switch (event->keyval) {
	case GDK_Return:
		if (check_preedit(text_view))
			return FALSE;
		if ((auto_indent && !(event->state &= GDK_SHIFT_MASK)) ||
			(!auto_indent && (event->state &= GDK_SHIFT_MASK))) {
			indent_real(text_view);
			return TRUE;
		}
		break;
	case GDK_Tab:
		if (event->state &= GDK_CONTROL_MASK) {
			toggle_tab_width(text_view);
			return TRUE;
		}
	case GDK_ISO_Left_Tab:
		if (event->state &= GDK_SHIFT_MASK)
			multi_line_unindent(buffer);
		else if (!check_selection_bound(buffer))
			break;
		else
			multi_line_indent(buffer);
		return TRUE;
	}
	return FALSE;
}

void indent_init(GtkWidget *text_view)
{
	indent_refresh_tab_width(text_view);
	g_signal_connect(G_OBJECT(text_view), "key-press-event",
		G_CALLBACK(cb_key_press_event), NULL);
}
