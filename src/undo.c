/*
 *  undo.c
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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "undo.h"

#define DV(x)

typedef struct {
	gint command;
	gint start;
	gint end;
	gboolean seq; // sequency flag
	gchar *str;
} UndoInfo;

enum {
	DEL = 0,
	BS,
	INS
};

static GList *undo_list = NULL;
static GList *redo_list = NULL;
static gint step_modif;
static GtkWidget *view;
static gboolean overwrite_mode = FALSE;
static guint prev_keyval = 0;//, keyval = 0;

static void cb_toggle_overwrite(void)
{
	overwrite_mode =! overwrite_mode;
DV(g_print("toggle-overwrite: %d\n", overwrite_mode));
}

static void undo_clear_undo_info(void)
{
	while (g_list_length(undo_list)) {
		g_free(((UndoInfo *)undo_list->data)->str);
		g_free(undo_list->data);
		undo_list = g_list_delete_link(undo_list, undo_list);
	}
//DV(g_print("undo_cb: Undo list cleared by %d\n", g_list_length(undo_list)));
}

static void undo_clear_redo_info(void)
{
	while (g_list_length(redo_list)) {
		g_free(((UndoInfo *)redo_list->data)->str);
		g_free(redo_list->data);
		redo_list = g_list_delete_link(redo_list, redo_list);
	}
//DV(g_print("undo_cb: Redo list cleared by %d\n", g_list_length(redo_list)));
}

static void undo_append_undo_info(GtkTextBuffer *buffer, gint command, gint start, gint end, gboolean seq)
{
	GtkTextIter start_iter, end_iter;
	UndoInfo *ui = g_malloc(sizeof(UndoInfo));
	
	gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, start);
	gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, end);
	ui->command = command;
	ui->start = start;
	ui->end = end;
	ui->seq = seq;
	ui->str = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
	undo_list = g_list_append(undo_list, ui);
DV(g_print("undo_cb: %d %s (%d-%d) %d\n", command, ui->str, start, end, seq));
	undo_clear_redo_info();
}
/*
static void cb_delete_range(GtkTextBuffer *buffer, GdkEventKey *event)
{
	GtkTextIter start_iter, end_iter;
	gint start, end;
	gboolean seq = FALSE;
	
	gtk_text_buffer_get_selection_bounds(buffer, &start_iter, &end_iter);
	start = gtk_text_iter_get_offset(&start_iter);
	end = gtk_text_iter_get_offset(&end_iter);
	if (start == end) {
		switch (event->keyval) {
		case GDK_BackSpace:
		case GDK_Delete:
			--start;
g_print("delkey: BS or DEL\n");		
			break;
		default:
			if (overwrite_mode) {
				++end;
				seq = TRUE;
			} else
				--start;
		}
	}
	undo_append_undo_info(buffer, DELETE, start, end, seq);
}
*/
static void cb_delete_range(GtkTextBuffer *buffer, GtkTextIter *start_iter, GtkTextIter *end_iter)
{
	gint start, end;
	gint command;
//	gboolean seq = FALSE;
	
	start = gtk_text_iter_get_offset(start_iter);
	end = gtk_text_iter_get_offset(end_iter);
	if (!keyval && prev_keyval)
		undo_set_sequency(TRUE);
	if (keyval == GDK_BackSpace)
		command = BS;
	else
		command = DEL;
	undo_append_undo_info(buffer, command, start, end, FALSE);//seq);
	
DV(g_print("delete-range: keyval = %d\n", keyval));
	prev_keyval = keyval;
	keyval = 0;
}

static void cb_insert_text(GtkTextBuffer *buffer, GtkTextIter *iter, gchar *str)
{
	gint start, end;/*, len;
	static gboolean after = FALSE, block = FALSE;
	
	if (!after) {
		after = TRUE;
		if (overwrite_mode) {
			len = g_utf8_strlen(str, -1);
			if (len > 1) {
				GtkTextIter *ovw_iter = gtk_text_iter_copy(iter);
				
				block = TRUE;
				g_signal_handlers_block_by_func(G_OBJECT(view), G_CALLBACK(cb_toggle_overwrite), NULL);
				g_signal_emit_by_name(G_OBJECT(view), "toggle-overwrite");
				gtk_text_iter_forward_chars(ovw_iter, len - 1);
				gtk_text_buffer_delete(buffer, iter, ovw_iter);
				gtk_text_iter_free(ovw_iter);
			}
		}
		return;
	}
	after = FALSE;
	if (block) {
		g_signal_emit_by_name(G_OBJECT(view), "toggle-overwrite");
		g_signal_handlers_unblock_by_func(G_OBJECT(view), G_CALLBACK(cb_toggle_overwrite), NULL);
		block = FALSE;
	}
*/	
	end = gtk_text_iter_get_offset(iter);
	start = end - g_utf8_strlen(str, -1);
	if (!keyval && prev_keyval)
		undo_set_sequency(TRUE);
	undo_append_undo_info(buffer, INS, start, end, FALSE);
	
DV(g_print("insert-text: keyval = %d\n", keyval));
	prev_keyval = keyval;
	keyval = 0;
}
/* v3
static void cb_insert_text(GtkTextBuffer *buffer, GtkTextIter *iter, gchar *str, gint len)
{
	static gint start, end;
	static gboolean after = FALSE;
	
	if (!after) {
		after = TRUE;
		start = gtk_text_iter_get_offset(iter);
DV(g_print("insert-text: len = %d\n", len));
		return;
	}
	after = FALSE;
	end = gtk_text_iter_get_offset(iter);
	if (!keyval && prev_keyval)
		undo_set_sequency(TRUE);
	undo_append_undo_info(buffer, INS, start, end, FALSE);
	
DV(g_print("insert-text: keyval = %d\n", keyval));
	prev_keyval = keyval;
	keyval = 0;
}
*//* v2 (cannot manage xim input)
static void cb_insert_text(GtkTextBuffer *buffer, GtkTextIter *iter, gchar *str, gint len)
{
	gint start, end;
//	gboolean seq = FALSE;
	
//	start = gtk_text_iter_get_offset(iter);
//	end = start + len;
	end = gtk_text_iter_get_offset(iter);
	start = end - len;
	if (!keyval && prev_keyval)
//		seq = TRUE;
//	if (!keyval)
		undo_set_sequency(TRUE);
	undo_append_undo_info(buffer, INS, start, end, FALSE);//seq);
	
DV(g_print("insert-text: keyval = %d\n", keyval));
	prev_keyval = keyval;
	keyval = 0;
}
*//* v1
static void cb_insert_text(GtkTextBuffer *buffer)
{
	GtkTextIter start_iter, end_iter;
	gboolean seq = FALSE;
	static gint start, end;
	static gboolean after = FALSE;
	
	gtk_text_buffer_get_selection_bounds(buffer, &start_iter, &end_iter);
	if (!after) {
		start = gtk_text_iter_get_offset(&start_iter);
		after = TRUE;
	} else {
g_print("insert-text\n");
		end = gtk_text_iter_get_offset(&start_iter);
		after = FALSE;
		if (gtk_text_iter_compare(&start_iter, &end_iter) != 0)
			seq = TRUE; // for selection bound over write
		undo_append_undo_info(buffer, INSERT, start, end, seq);
	}
	g_print("keyval: %d\n", keyval);
	keyval = 0;
}
*/
static void set_main_window_title_with_asterisk(gboolean flag)
{
	const gchar *old_title =
		gtk_window_get_title(GTK_WINDOW(gtk_widget_get_toplevel(view)));
	gchar *new_title;
	
	if (flag)
		new_title = g_strconcat("*", old_title, NULL);
	else {
		GString *gstr = g_string_new(old_title);
		gstr = g_string_erase(gstr, 0, 1);
		new_title = g_string_free(gstr, FALSE);
	}
	gtk_window_set_title(GTK_WINDOW(gtk_widget_get_toplevel(view)), new_title);
	g_free(new_title);
}
/*
static void set_main_window_title_toggle_asterisk(void)
{
	const gchar *old_title =
		gtk_window_get_title(GTK_WINDOW(gtk_widget_get_toplevel(view)));
	gchar *new_title;
	
	if (g_str_has_prefix(old_title, "*")) {
		GString *gstr = g_string_new(old_title);
		gstr = g_string_erase(gstr, 0, 1);
		new_title = g_string_free(gstr, FALSE);
	} else {
		new_title = g_strconcat("*", old_title, NULL);
	}
	gtk_window_set_title(GTK_WINDOW(gtk_widget_get_toplevel(view)), new_title);
	g_free(new_title);
}
*/
static void cb_modified_changed(void)
{
	set_main_window_title_with_asterisk(TRUE);
}

static gboolean cb_key_press_event(GtkWidget *widget, GdkEventKey *event)
{
/*	switch (event->keyval) {
	case GDK_BackSpace:
		g_print("BS pushed\n");
		break;
	case GDK_Delete:
		g_print("DEL pushed\n");
		break;
	default:
		g_print("any other key pushed\n");
	} */
//	if (keyval == GDK_Return) g_print("[Return]");
//	g_print("cb_key_press_event: %d\n", keyval);
	if (event->keyval) keyval = event->keyval;
	
	return FALSE;
}

void undo_reset_step_modif(void)
{
	step_modif = g_list_length(undo_list);
DV(g_print("undo_reset_step_modif: Reseted step_modif by %d\n", step_modif));
}

static void undo_check_step_modif(GtkTextBuffer *buffer)
{
	if (g_list_length(undo_list) == step_modif) {
		g_signal_handlers_block_by_func(G_OBJECT(buffer), G_CALLBACK(cb_modified_changed), NULL);
		gtk_text_buffer_set_modified(buffer, FALSE);
		g_signal_handlers_unblock_by_func(G_OBJECT(buffer), G_CALLBACK(cb_modified_changed), NULL);
		set_main_window_title_with_asterisk(FALSE);
	}
}

static gint undo_connect_signal(GtkTextBuffer *buffer)
{
	g_signal_connect(G_OBJECT(buffer), "delete-range",
		G_CALLBACK(cb_delete_range), buffer);
//	g_signal_connect(G_OBJECT(buffer), "insert-text",
//		G_CALLBACK(cb_insert_text), buffer);
//	g_signal_connect_after(G_OBJECT(buffer), "delete-range",
//		G_CALLBACK(cb_delete_range), buffer);
	g_signal_connect_after(G_OBJECT(buffer), "insert-text",
		G_CALLBACK(cb_insert_text), buffer);
	return 
	g_signal_connect(G_OBJECT(buffer), "modified-changed",
		G_CALLBACK(cb_modified_changed), NULL);
}

void undo_init(GtkWidget *textview, GtkTextBuffer *buffer)
{
	static guint flag = 0;
	
	if (undo_list)
		undo_clear_undo_info();
	if (redo_list)
		undo_clear_redo_info();
	undo_reset_step_modif();
DV(g_print("undo_init: list reseted\n"));
	
	if (!flag) {
		g_signal_connect(G_OBJECT(textview), "key-press-event",
			G_CALLBACK(cb_key_press_event), NULL);
		g_signal_connect(G_OBJECT(textview), "toggle-overwrite",
			G_CALLBACK(cb_toggle_overwrite), NULL);
		view = textview;
		flag = undo_connect_signal(buffer);
		keyval = 0;
	}
}

gint undo_block_signal(GtkTextBuffer *buffer)
{
	return 
	g_signal_handlers_block_by_func(G_OBJECT(buffer), G_CALLBACK(cb_delete_range), buffer) +
//	g_signal_handlers_block_by_func(G_OBJECT(buffer), G_CALLBACK(cb_modified_changed), buffer) +
	g_signal_handlers_block_by_func(G_OBJECT(buffer), G_CALLBACK(cb_insert_text), buffer);
}

gint undo_unblock_signal(GtkTextBuffer *buffer)
{
	return 
	g_signal_handlers_unblock_by_func(G_OBJECT(buffer), G_CALLBACK(cb_delete_range), buffer) +
//	g_signal_handlers_unblock_by_func(G_OBJECT(buffer), G_CALLBACK(cb_modified_changed), buffer) +
	g_signal_handlers_unblock_by_func(G_OBJECT(buffer), G_CALLBACK(cb_insert_text), buffer);
}

gint undo_disconnect_signal(GtkTextBuffer *buffer) //must be not needed
{
	return 
	g_signal_handlers_disconnect_by_func(G_OBJECT(buffer), G_CALLBACK(cb_delete_range), buffer) +
//	g_signal_handlers_disconnect_by_func(G_OBJECT(buffer), G_CALLBACK(cb_modified_changed), buffer) +
	g_signal_handlers_disconnect_by_func(G_OBJECT(buffer), G_CALLBACK(cb_insert_text), buffer);
}

void undo_set_sequency(gboolean seq)
{
	UndoInfo *ui;
	
	if (g_list_length(undo_list)) {
		ui = g_list_last(undo_list)->data;
		ui->seq = seq;
	}
}

gboolean undo_undo(GtkTextBuffer *buffer)
{
	GtkTextIter start_iter, end_iter;
	UndoInfo *ui;
	
	if (g_list_length(undo_list)) {
		undo_block_signal(buffer);
		ui = g_list_last(undo_list)->data;
		gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, ui->start);
		switch (ui->command) {
		case INS:
			gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, ui->end);
			gtk_text_buffer_delete(buffer, &start_iter, &end_iter);
			break;
		default:
			gtk_text_buffer_insert(buffer, &start_iter, ui->str, -1);
		}
		redo_list = g_list_append(redo_list, ui);
		undo_list = g_list_delete_link(undo_list, g_list_last(undo_list));
DV(g_print("cb_edit_undo: undo list left %d\n", g_list_length(undo_list)));
		undo_unblock_signal(buffer);
		undo_check_step_modif(buffer);
		if (g_list_length(undo_list))
			if (((UndoInfo *)g_list_last(undo_list)->data)->seq)
				return TRUE;
		if (ui->command == DEL)
			gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, ui->start);
		gtk_text_buffer_place_cursor(buffer, &start_iter);
		gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &start_iter,
			0.1, FALSE, 0.5, 0.5);
	}
	return FALSE;
}

gboolean undo_redo(GtkTextBuffer *buffer)
{
	GtkTextIter start_iter, end_iter;
	UndoInfo *ri;
	
	if (g_list_length(redo_list)) {
		undo_block_signal(buffer);
		ri = g_list_last(redo_list)->data;
		gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, ri->start);
		switch (ri->command) {
		case INS:
			gtk_text_buffer_insert(buffer, &start_iter, ri->str, -1);
			break;
		default:
			gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, ri->end);
			gtk_text_buffer_delete(buffer, &start_iter, &end_iter);
		}
		undo_list = g_list_append(undo_list, ri);
		redo_list = g_list_delete_link(redo_list, g_list_last(redo_list));
DV(g_print("cb_edit_redo: redo list left %d\n", g_list_length(redo_list)));
		undo_unblock_signal(buffer);
		undo_check_step_modif(buffer);
		if (ri->seq)
			return TRUE;
		gtk_text_buffer_place_cursor(buffer, &start_iter);
		gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(view), &start_iter,
			0.1, FALSE, 0.5, 0.5);
	}
	return FALSE;
}
