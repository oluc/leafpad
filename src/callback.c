/*
 *  callback.c
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

#include "leafpad.h"

static gint check_text_modification(StructData *sd)
{
	gchar *basename, *str;
	gint res;
	
	GtkTextBuffer *textbuffer = sd->mainwin->textbuffer;
	
	if (gtk_text_buffer_get_modified(textbuffer)) {
		basename = get_current_file_basename(sd->fi->filepath);
		str = g_strdup_printf(_("Save changes to '%s'?"), basename);
		res = run_dialog_message_question(sd->mainwin->window, str);
		g_free(str);
		g_free(basename);
		switch (res) {
		case GTK_RESPONSE_CANCEL:
			return -1;
		case GTK_RESPONSE_YES:
			if (cb_file_save(sd))
				return -1;
		}
	}
	
	return 0;
}

static void set_text_selection_bound(GtkTextBuffer *buffer, gint start, gint end)
{
	GtkTextIter start_iter;
	GtkTextIter end_iter;
	
	gtk_text_buffer_get_iter_at_offset(buffer, &start_iter, start);
	if (end < 0)
		gtk_text_buffer_get_end_iter(buffer, &end_iter);
	else
		gtk_text_buffer_get_iter_at_offset(buffer, &end_iter, end);
	gtk_text_buffer_place_cursor(buffer, &end_iter);
	gtk_text_buffer_move_mark_by_name(buffer, "selection_bound", &start_iter);
}

void cb_file_new(StructData *sd)
{
	GtkTextBuffer *textbuffer = sd->mainwin->textbuffer;
	
	if (!check_text_modification(sd)) {
		undo_block_signal(textbuffer);
		gtk_text_buffer_set_text(textbuffer, "", 0);
		gtk_text_buffer_set_modified(textbuffer, FALSE);
		sd->fi->filepath = NULL;
		sd->fi->charset = NULL;
		sd->fi->line_ending = LF;
		set_main_window_title(sd);
		undo_unblock_signal(textbuffer);
		undo_init(sd->mainwin->textview, textbuffer, sd->mainwin->menubar);
	}
}

void cb_file_new_window(StructData *sd)
{
	g_spawn_command_line_async(PACKAGE, NULL);
}

void cb_file_open(StructData *sd)
{
	FileInfo *fi;
	
	if (!check_text_modification(sd)) {
		fi = get_file_info_by_selector(sd->mainwin->window, OPEN, sd->fi);
		if (fi) {
			if (file_open_real(sd->mainwin->textview, fi)) {
				g_free(fi);
			} else {
				g_free(sd->fi);
				sd->fi = fi;
				set_main_window_title(sd);
				undo_init(sd->mainwin->textview, sd->mainwin->textbuffer, sd->mainwin->menubar);
			}
		}
	}
}

gint cb_file_save(StructData *sd)
{
	if (sd->fi->filepath == NULL)
		return cb_file_save_as(sd);
	if (!file_save_real(sd->mainwin->textview, sd->fi)) {
		set_main_window_title(sd);
		undo_reset_step_modif();
		return 0;
	}
	return -1;
}

gint cb_file_save_as(StructData *sd)
{
	FileInfo *fi;
	
	fi = get_file_info_by_selector(sd->mainwin->window, SAVE, sd->fi);
	if (fi) {
		if (file_save_real(sd->mainwin->textview, fi))
			g_free(fi);
		else {
			g_free(sd->fi);
			sd->fi = fi;
			set_main_window_title(sd);
			undo_reset_step_modif();
			return 0;
		}
	}
	return -1;
}
/*
void cb_file_page_setup(StructData *sd)
{
	;
}

void cb_file_print(StructData *sd)
{
	;
}
*/
void cb_file_quit(StructData *sd)
{
	if (!check_text_modification(sd))
		gtk_main_quit();
}

void cb_edit_undo(StructData *sd)
{
	while (undo_undo(sd->mainwin->textbuffer)) {}
}

void cb_edit_redo(StructData *sd)
{
	while (undo_redo(sd->mainwin->textbuffer)) {
		undo_set_sequency(TRUE);
	}
}

void cb_edit_cut(StructData *sd)
{
	gtk_text_buffer_cut_clipboard(sd->mainwin->textbuffer,
		gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), TRUE);
	menu_toggle_paste_item(); // TODO: remove this line
}

void cb_edit_copy(StructData *sd)
{
	gtk_text_buffer_copy_clipboard(sd->mainwin->textbuffer,
		gtk_clipboard_get(GDK_SELECTION_CLIPBOARD));
	menu_toggle_paste_item(); // TODO: remove this line
}

void cb_edit_paste(StructData *sd)
{
	gtk_text_buffer_paste_clipboard(sd->mainwin->textbuffer,
		gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), NULL, TRUE);
	gtk_text_view_scroll_mark_onscreen(
		GTK_TEXT_VIEW(sd->mainwin->textview),
		gtk_text_buffer_get_insert(sd->mainwin->textbuffer));
}

void cb_edit_delete(StructData *sd)
{
	gtk_text_buffer_delete_selection(sd->mainwin->textbuffer,
		TRUE, TRUE);
}

void cb_edit_select_all(StructData *sd)
{
	set_text_selection_bound(sd->mainwin->textbuffer, 0, -1);
}

static void activate_quick_find(StructData *sd)
{
	GtkItemFactory *ifactory;
	static gboolean flag = FALSE;
	
	if (!flag) {
		ifactory = gtk_item_factory_from_widget(sd->mainwin->menubar);
		gtk_widget_set_sensitive(
			gtk_item_factory_get_widget(ifactory, "/Search/Find Next"),
			TRUE);
		gtk_widget_set_sensitive(
			gtk_item_factory_get_widget(ifactory, "/Search/Find Previous"),
			TRUE);
		flag = TRUE;
	}
}
	
void cb_search_find(StructData *sd)
{
	if (run_dialog_search(sd->mainwin->textview, 0) == GTK_RESPONSE_OK)
		activate_quick_find(sd);
}

void cb_search_find_next(StructData *sd)
{
	document_search_real(sd->mainwin->textview, 1);
}

void cb_search_find_prev(StructData *sd)
{
	document_search_real(sd->mainwin->textview, -1);
}

void cb_search_replace(StructData *sd)
{
	if (run_dialog_search(sd->mainwin->textview, 1) == GTK_RESPONSE_OK)
		activate_quick_find(sd);
}

void cb_search_jump_to(StructData *sd)
{
	run_dialog_jump_to(sd->mainwin->textview);
}

void cb_option_font(StructData *sd)
{
	change_text_font_by_selector(sd->mainwin->textview);
}

void cb_option_word_wrap(StructData *sd, guint action, GtkWidget *widget)
{
	GtkItemFactory *ifactory;
	gboolean check;
	
	ifactory = gtk_item_factory_from_widget(widget);
	check = gtk_check_menu_item_get_active(
		GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(ifactory, "/Options/Word Wrap")));
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(sd->mainwin->textview),
		check ? GTK_WRAP_WORD : GTK_WRAP_NONE);
}

void cb_option_line_numbers(StructData *sd, guint action, GtkWidget *widget)
{
	GtkItemFactory *ifactory;
	gboolean state;
	
	ifactory = gtk_item_factory_from_widget(widget);
	state = gtk_check_menu_item_get_active(
		GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(ifactory, "/Options/Line Numbers")));
	show_line_numbers(sd->mainwin->textview, state);
}

void cb_option_auto_indent(StructData *sd, guint action, GtkWidget *widget)
{
	GtkItemFactory *ifactory;
	gboolean state;
	
	ifactory = gtk_item_factory_from_widget(widget);
	state = gtk_check_menu_item_get_active(
		GTK_CHECK_MENU_ITEM(gtk_item_factory_get_item(ifactory, "/Options/Auto Indent")));
	indent_set_state(state);
}

void cb_help_about(StructData *sd)
{
	run_dialog_about(sd->mainwin->window,
		PACKAGE_NAME,
		PACKAGE_VERSION,
		_("GTK+ based simple text editor"),
		"Copyright &#169; 2004 Tarot Osuji",
		ICONDIR G_DIR_SEPARATOR_S PACKAGE ".png");
}
