/*
 *  file.c
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "intl.h"
#include "encoding.h"
#include "dialog.h"
#include "undo.h"
#include "file.h"

gint file_open_real(GtkWidget *textview, FileInfo *fi)
{
	gchar *contents;
	gsize length;
	GError *err = NULL;
	gchar *charset;
	gchar *str = NULL;
	GtkTextIter iter;
//	const CharsetInfo *ci;
//	gint i = 0;
	
	GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	
	if (!g_file_get_contents(fi->filepath, &contents, &length, &err)) {
		if (g_file_test(fi->filepath, G_FILE_TEST_EXISTS)) {
			run_dialog_message(gtk_widget_get_toplevel(textview), GTK_MESSAGE_ERROR, err->message);
			g_error_free(err);
			return -1;
		}
		g_error_free(err);
		contents = g_strdup("");
	} 
/*	if (g_file_test(fi->filepath, G_FILE_TEST_IS_DIR)) {
		run_dialog_message(gtk_widget_get_toplevel(textview), GTK_MESSAGE_ERROR,
			_("'%s' is a directory"), fi->filepath);
		return -1;
	}
*/	
//	if (fi->line_ending == NULL)
		fi->line_ending = detect_line_ending(contents);
	if (fi->line_ending != LF)
		convert_line_ending_to_lf(contents);
	
	if (fi->charset)
		charset = fi->charset;
	else
		charset = detect_charset(contents);
	if (!charset || (strcmp(charset, "US-ASCII") == 0)) {
		if (fi->manual_charset)
			charset = fi->manual_charset;
		else
			charset = (gchar *) get_default_charset();
	}

	if (length)
		do {
			if (err) {
				if (fi->manual_charset) {
					fi->manual_charset = NULL;
					if (fi->charset == NULL)
						charset = (gchar *) get_default_charset();
				}
				if (strcmp(charset, "GB2312") == 0)
					charset = "GB18030";
				else if (strcasecmp(charset, "Big5") == 0)
					charset = "Big5-HKSCS";
				else if (strncasecmp(charset, "Shift", 5) == 0)
					charset = "Windows-31J";
			/*	else if (strcasecmp(charset, "EUC-KR") == 0)
					charset = "UHC"; */ // not listed on IANA :-I
				else
					charset = "ISO-8859-1";
				g_error_free(err);
				err = NULL;
			}
			str = g_convert(contents, -1, "UTF-8", charset, NULL, NULL, &err);
		} while (err);
	else
		str = g_strdup("");
	
	fi->charset = charset;
	
/*	fi->manual_charset = charset;
	while ((get_charset_info_from_index(i)) != NULL) {
		ci = get_charset_info_from_index(i);
		if (fi->manual_charset == ci->charset) {
			fi->manual_charset = NULL;
			break;
		}
		i++;
	}
*/	
	if (fi->manual_charset != fi->charset) //
		fi->manual_charset = NULL; //
	g_free(contents);
	
//	undo_disconnect_signal(textbuffer);
	undo_block_signal(textbuffer);
	gtk_text_buffer_set_text(textbuffer, "", 0);
	gtk_text_buffer_get_start_iter(textbuffer, &iter);
	gtk_text_buffer_insert(textbuffer, &iter, str, strlen(str));
	gtk_text_buffer_get_start_iter(textbuffer, &iter);
	gtk_text_buffer_place_cursor(textbuffer, &iter);
	gtk_text_buffer_set_modified(textbuffer, FALSE);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(textview), &iter, 0, FALSE, 0, 0);
	g_free(str);
	undo_unblock_signal(textbuffer);
	
	return 0;
}

gint file_save_real (GtkWidget *textview, FileInfo *fi)
{
	FILE *fp;
	GtkTextIter start, end;
	gchar *str;
	gint rbytes, wbytes;
	GError *err = NULL;
	
	GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
	
	gtk_text_buffer_get_start_iter(textbuffer, &start);
	gtk_text_buffer_get_end_iter(textbuffer, &end);	
	str = gtk_text_buffer_get_text(textbuffer, &start, &end, TRUE);
	
	switch (fi->line_ending) {
	case CR:
		convert_line_ending(&str, CR);
		break;
	case CR+LF:
		convert_line_ending(&str, CR+LF);
	}
	
	if (!fi->charset)
		fi->charset = (gchar *)get_default_charset();
	str = g_convert(str, -1, fi->charset, "UTF-8", &rbytes, &wbytes, &err);
	if (err) {
		switch (err->code) {
		case G_CONVERT_ERROR_ILLEGAL_SEQUENCE:
			run_dialog_message(gtk_widget_get_toplevel(textview),
				GTK_MESSAGE_ERROR, _("Can't convert codeset to '%s'"), fi->charset);
			break;
		default:
			run_dialog_message(gtk_widget_get_toplevel(textview),
				GTK_MESSAGE_ERROR, err->message);
		}
		g_error_free(err);
		return -1;
	}
	
	fp = fopen(fi->filepath, "w");
	if (!fp) {
		run_dialog_message(gtk_widget_get_toplevel(textview),
			GTK_MESSAGE_ERROR, _("Can't open file to write"));
		return -1;
	}
	if (fputs (str, fp) == EOF) {
		run_dialog_message(gtk_widget_get_toplevel(textview),
			GTK_MESSAGE_ERROR, _("Can't write file"));
		return -1;
	}
	
	gtk_text_buffer_set_modified(textbuffer, FALSE);
	fclose(fp);
	g_free(str);
	
	return 0;
}
/*
gchar* get_file_name_by_selector(GtkWidget *window, const gchar *title, gchar *default_filepath)
{
	GtkWidget *filesel;
	gchar *filename = NULL;
	
	filesel = gtk_file_selection_new(title);
	gtk_window_set_transient_for(GTK_WINDOW(filesel), GTK_WINDOW(window));
	
	if (current_filepath)
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(filesel), default_filepath);
	
	if (gtk_dialog_run(GTK_DIALOG(filesel)) == GTK_RESPONSE_OK)
		filename = (gchar *) gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
	
	gtk_widget_destroy(filesel);
	return filename;
}
*/
static gchar *selected_charset;
static gint selected_line_ending;
static GtkWidget *option_menu_charset;
static GtkWidget *menu_manual_charset;
static gchar *manual_charset;
static gboolean flag_manual_charset = FALSE;
static gboolean flag_manual_charset_disp;
static gint manual_charset_num;
static gint menu_num_history;
static GtkWidget *filesel;

static gchar *le_str[] = {
	"UNIX (LF)",
	"DOS (CR+LF)",
	"Mac (CR)"
};

static void init_manual_charset(void)
{
	gchar *str;
	
	if (!flag_manual_charset_disp)
		menu_manual_charset = gtk_menu_item_new_with_label(_("Other Codeset..."));
	flag_manual_charset_disp = TRUE;
	if (manual_charset) {
		GtkWidget *child = GTK_BIN(menu_manual_charset)->child;
		str = g_strdup_printf(_("Other Codeset (%s)"), manual_charset);
		gtk_label_set_text(GTK_LABEL(child), str);
		g_free(str);
	}
}

static void run_dialog_enter_charset(void)
{
	GtkWidget* dialog;
	GtkWidget* hbox;
	GtkWidget* label;
	GtkWidget* entry;
	GError *err = NULL;
	gchar *str;
	gboolean changed = FALSE;
	
	dialog = gtk_dialog_new_with_buttons(_("Enter Codeset"), GTK_WINDOW(filesel),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OK, GTK_RESPONSE_OK,
			NULL);
	hbox = gtk_hbox_new(FALSE, 0);
	 gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);
	 gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, FALSE, FALSE, 0);
	label = gtk_label_new_with_mnemonic(_("Code_set: "));
	 gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	entry = gtk_entry_new();
//	 gtk_entry_set_width_chars(GTK_ENTRY(entry), 12);
	 gtk_box_pack_end(GTK_BOX(hbox), entry, FALSE, FALSE, 0);
	 gtk_label_set_mnemonic_widget(GTK_LABEL(label), entry);
	if (manual_charset)
		gtk_entry_set_text(GTK_ENTRY(entry), manual_charset);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
//	gtk_dialog_set_response_sensitive(GTK_DIALOG(dialog), GTK_RESPONSE_OK, TRUE);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	gtk_widget_show_all(hbox);
	
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
		g_convert("TEST", -1, "UTF-8", gtk_entry_get_text(GTK_ENTRY(entry)), NULL, NULL, &err);
		if (err) {
			g_error_free(err);
			str = g_strdup_printf(_("'%s' is not supported"), gtk_entry_get_text(GTK_ENTRY(entry)));
			run_dialog_message(filesel, GTK_MESSAGE_ERROR, str);
			g_free(str);
		} else if (strlen(gtk_entry_get_text(GTK_ENTRY(entry)))) {
			g_free(manual_charset);
			manual_charset = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
			changed = TRUE;
		}
	}
	if (!changed)
		gtk_option_menu_set_history(
			GTK_OPTION_MENU(option_menu_charset), menu_num_history);
	
	gtk_widget_destroy(dialog);
}

static void cb_select_charset(gint num)
{
	const CharsetInfo *ci;// = g_malloc(sizeof(CharsetInfo));
	
	if (num == manual_charset_num) {
		/* Input Dialog */
		run_dialog_enter_charset();
		/* Confirm valid charset */
		init_manual_charset();
		selected_charset = manual_charset;
		flag_manual_charset = TRUE;
	} else {
		flag_manual_charset = FALSE;
		if (num < 0)
			selected_charset = NULL;
		else {
			ci = get_charset_info_from_index(num);
			selected_charset = ci->charset;
		}
	}
	menu_num_history = gtk_option_menu_get_history(GTK_OPTION_MENU(option_menu_charset));
}

static void cb_select_line_ending(gchar *line_ending)
{
	if (line_ending == le_str[1])
		selected_line_ending = CR+LF;
	else if (line_ending == le_str[2])
		selected_line_ending = CR;
	else
		selected_line_ending = LF;
}

FileInfo *get_file_info_by_selector(GtkWidget *window, gint mode, FileInfo *fi)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *option_menu_line_ending;
	GtkWidget *menu_charset;
	GtkWidget *menu_line_ending;
	GtkWidget *menu_items;
	gchar *title;
	
	const CharsetInfo *ci;// = g_malloc(sizeof(CharsetInfo));
	gint menu_num = -1;
	gchar *str;
	gint i;
	
	FileInfo *selected = NULL;
	gchar *selected_filename; /* For confirm dialog */
	selected_charset = NULL;
	selected_line_ending = fi->line_ending;
	manual_charset = NULL;
	
	if (fi->manual_charset)
		manual_charset = g_strdup(fi->manual_charset);
	
	if (mode == OPEN)
		title = N_("Open");
	else
		title = N_("Save As");
	
	filesel = gtk_file_selection_new(_(title));
	gtk_window_set_transient_for(GTK_WINDOW(filesel), GTK_WINDOW(window));
	
	hbox = gtk_hbox_new(FALSE, 0);
// TODO: remove OK signal
	option_menu_charset = gtk_option_menu_new();
	label = gtk_label_new_with_mnemonic(_("Ch_aracter Coding: "));
	gtk_label_set_mnemonic_widget(GTK_LABEL(label), option_menu_charset);
	
	if (mode == SAVE) {	
		option_menu_line_ending = gtk_option_menu_new();
		menu_line_ending = gtk_menu_new();
		for (i = 0; i <= 2; i++) {
			menu_items = gtk_menu_item_new_with_label(le_str[i]);
			gtk_menu_append(GTK_MENU(menu_line_ending), menu_items);
			g_signal_connect_swapped(G_OBJECT(menu_items),
				"activate", G_CALLBACK(cb_select_line_ending), le_str[i]);
			gtk_widget_show(menu_items);
		}
		gtk_box_pack_end(GTK_BOX(hbox), option_menu_line_ending, FALSE, FALSE, 0);
		gtk_option_menu_set_menu(GTK_OPTION_MENU(option_menu_line_ending), menu_line_ending);
		
		if (fi->line_ending == CR+LF)
			gtk_option_menu_set_history(GTK_OPTION_MENU(option_menu_line_ending), 1);
		else if (fi->line_ending == CR)
			gtk_option_menu_set_history(GTK_OPTION_MENU(option_menu_line_ending), 2);
	}
	
	gtk_box_pack_end(GTK_BOX(hbox), option_menu_charset, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	
	menu_charset = gtk_menu_new();
	if (mode == OPEN) {
		menu_items = gtk_menu_item_new_with_label(_("Auto-Detect"));
		gtk_menu_append(GTK_MENU(menu_charset), menu_items);
		g_signal_connect_swapped(G_OBJECT(menu_items),
				"activate", G_CALLBACK(cb_select_charset), (gpointer) -1);
		gtk_widget_show(menu_items);
	}
	
	i = 0;
	while ((get_charset_info_from_index(i)) != NULL)
	{
		ci = get_charset_info_from_index(i);
		str = g_strdup_printf("%s (%s)", _(ci->desc), ci->charset);
		menu_items = gtk_menu_item_new_with_label(str);
		g_free(str);
		
		gtk_menu_append(GTK_MENU(menu_charset), menu_items);
		g_signal_connect_swapped(G_OBJECT(menu_items),
			"activate", G_CALLBACK(cb_select_charset), (gpointer) i);
		gtk_widget_show(menu_items);
		
		if (fi->charset && (menu_num < 0)) {
			if (strcasecmp(ci->charset, fi->charset) == 0) {
				if (mode)
					menu_num = i - mode + 1;
				else
					menu_num = -2;
			}
		}
		i++;
	}
	
	if (fi->charset && (menu_num == -1))
		manual_charset = g_strdup(fi->charset);
	
	flag_manual_charset_disp = FALSE;
	init_manual_charset();
	gtk_menu_append(GTK_MENU(menu_charset), menu_manual_charset);
	manual_charset_num = i;
	g_signal_connect_swapped(G_OBJECT(menu_manual_charset),
			"activate", G_CALLBACK(cb_select_charset), (gpointer) manual_charset_num);
	gtk_widget_show(menu_manual_charset);
	
	gtk_option_menu_set_menu(GTK_OPTION_MENU(option_menu_charset), menu_charset);
	if (mode && (menu_num >= 0)) {
		gtk_option_menu_set_history(GTK_OPTION_MENU(option_menu_charset), menu_num);
		selected_charset = fi->charset;
	}
	else if (manual_charset) {
		if (flag_manual_charset || mode) {
			gtk_option_menu_set_history(
				GTK_OPTION_MENU(option_menu_charset), manual_charset_num - mode + 1);
			selected_charset = manual_charset;
		}
	}
/*	else if (manual_charset && fi->charset) {
		if (flag_manual_charset || (mode && strcasecmp(manual_charset, fi->charset) == 0)) {
			gtk_option_menu_set_history(
				GTK_OPTION_MENU(option_menu_charset), manual_charset_num - mode + 1);
			selected_charset = fi->charset;
		}
	}
*/	
	menu_num_history = gtk_option_menu_get_history(GTK_OPTION_MENU(option_menu_charset));
	
	gtk_box_pack_end(GTK_BOX(GTK_FILE_SELECTION(filesel)->main_vbox),
		hbox, FALSE, FALSE, 5);
	gtk_widget_show_all(hbox);
	
	if (fi->filepath)
		gtk_file_selection_set_filename(GTK_FILE_SELECTION(filesel), fi->filepath);
	
/*	if (gtk_dialog_run(GTK_DIALOG(filesel)) == GTK_RESPONSE_OK) {
		selected = g_malloc(sizeof(FileInfo));
		selected->filepath = (gchar *) gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
		selected->charset = selected_charset;
		selected->line_ending = selected_line_ending;
	}
*/	
	do {
		if (gtk_dialog_run(GTK_DIALOG(filesel)) == GTK_RESPONSE_OK) {
			selected_filename = (gchar *) gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel));
			if (mode == SAVE && g_file_test(selected_filename, G_FILE_TEST_EXISTS)) {
				gchar *basename, *str;
				gint res;
				basename = g_path_get_basename(selected_filename);
				str = g_strdup_printf(_("'%s' already exists. Overwrite?"), basename);
				res = run_dialog_message_question(filesel, str);
				g_free(str);
				g_free(basename);
				switch (res) {
				case GTK_RESPONSE_CANCEL:
					gtk_widget_hide(filesel);
				case GTK_RESPONSE_NO:
					continue;
				}
			}
			selected = g_malloc(sizeof(FileInfo));
			selected->filepath = selected_filename;
			selected->charset = selected_charset;
			selected->line_ending = selected_line_ending;
			selected->manual_charset = manual_charset;
			gtk_widget_hide(filesel);
		} else
			gtk_widget_hide(filesel);
	} while (GTK_WIDGET_VISIBLE(filesel));
	
	gtk_widget_destroy(filesel);
	
	return selected;
}
