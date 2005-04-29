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

#include "leafpad.h"

#ifdef HAVE_LPR

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#if !GTK_CHECK_VERSION(2, 4, 0)
#	define gtk_dialog_set_has_separator(Dialog, Setting)
#endif

static gchar *printer_name = NULL;
static gint print_num = 1;
static gboolean print_format = FALSE;

static gint write_tmp(gint fd)
{
	FILE *fp;
	GtkTextIter start, end;
	gchar *str;
	gint rbytes, wbytes;
	GError *error = NULL;
	
	GtkTextBuffer *buffer = pub->mw->buffer;
	
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);	
	str = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
	
	str = g_convert(str, -1, get_default_charset(), "UTF-8", &rbytes, &wbytes, &error);
	if (error) {
		switch (error->code) {
		case G_CONVERT_ERROR_ILLEGAL_SEQUENCE:
			run_dialog_message(pub->mw->window,
				GTK_MESSAGE_ERROR, _("Can't convert codeset to current locale"));
			break;
		default:
			run_dialog_message(pub->mw->window,
				GTK_MESSAGE_ERROR, error->message);
		}
		g_error_free(error);
		return -1;
	}
	
	fp = fdopen(fd, "w");
	if (fputs(str, fp) == EOF) {
		run_dialog_message(pub->mw->window,
			GTK_MESSAGE_ERROR, _("Can't write temporary file"));
		return -1;
	}
	
	fclose(fp);
	g_free(str);
	
	return 0;
}

static void toggle_sensitivity(GtkWidget *entry)
{
	gtk_dialog_set_response_sensitive(
		GTK_DIALOG(gtk_widget_get_toplevel(entry)), GTK_RESPONSE_OK,
		strlen(gtk_entry_get_text(GTK_ENTRY(entry))) ? TRUE : FALSE);
}

static void toggle_check(GtkToggleButton *toggle)
{
	print_format = gtk_toggle_button_get_active(toggle);
}

static gint run_dialog_print(void)
{
	GtkWidget *dialog;
	GtkWidget *table;
	GtkWidget *label_num, *label_name;
	GtkWidget *spinner, *entry, *check;
	GtkAdjustment *spinner_adj;
	gint retval;
	
	dialog = gtk_dialog_new_with_buttons(_("Print"),
		GTK_WINDOW(pub->mw->window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_PRINT, GTK_RESPONSE_OK,
		NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
/*	
	hbox = gtk_hbox_new(TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 8);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, FALSE, FALSE, 0);
*/	
	table = gtk_table_new(3, 2, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 8);
	gtk_table_set_col_spacings(GTK_TABLE(table), 8);
	gtk_container_set_border_width(GTK_CONTAINER(table), 8);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table, FALSE, FALSE, 0);
	
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), printer_name);
	gtk_entry_set_width_chars(GTK_ENTRY(entry), 16);
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	label_name = gtk_label_new_with_mnemonic(_("Printer _name:"));
	gtk_label_set_mnemonic_widget(GTK_LABEL(label_name), entry);
	gtk_misc_set_alignment(GTK_MISC(label_name), 0, 0.5);
	g_signal_connect_after(G_OBJECT(entry), "changed",
		G_CALLBACK(toggle_sensitivity), NULL);
	gtk_table_attach_defaults(GTK_TABLE(table), label_name, 0, 1, 1, 2);
	gtk_table_attach_defaults(GTK_TABLE(table), entry, 1, 2, 1, 2);
	
	spinner_adj = (GtkAdjustment *)gtk_adjustment_new(1, 1, 100, 1, 1, 0);
	spinner = gtk_spin_button_new(spinner_adj, 1, 0);
	gtk_entry_set_width_chars(GTK_ENTRY(spinner), 8);
	gtk_entry_set_activates_default(GTK_ENTRY(spinner), TRUE);
	label_num = gtk_label_new_with_mnemonic(_("N_umber of copies:"));
	gtk_label_set_mnemonic_widget(GTK_LABEL(label_num), spinner);
	gtk_misc_set_alignment(GTK_MISC(label_num), 0, 0.5);
	gtk_table_attach_defaults(GTK_TABLE(table), label_num, 0, 1, 0, 1);
	gtk_table_attach_defaults(GTK_TABLE(table), spinner, 1, 2, 0, 1);

	check = gtk_check_button_new_with_mnemonic(_("_Format page"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), print_format);
	g_signal_connect(GTK_OBJECT(check), "toggled", G_CALLBACK(toggle_check), NULL);
	gtk_table_attach_defaults(GTK_TABLE(table), check, 0, 2, 2, 3);
	
	gtk_widget_show_all(table);
	
	retval = gtk_dialog_run(GTK_DIALOG(dialog));
	if (retval == GTK_RESPONSE_OK) {
		g_free(printer_name);
		printer_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
	}
	gtk_widget_destroy (dialog);
	
	return retval;
}

static void init_vars(void)
{
	if (g_getenv("PRINTER"))
		printer_name = g_strdup(g_getenv("PRINTER"));
	else
		printer_name = g_strdup("lp");
}

void create_print_session(void)
{
	gint fd;
	gchar *tmp_filename;
	gchar *basename;
	gchar *comline;
	gchar *std_out;
	gchar *std_err;
	gint status;
	GError *error = NULL;
	
	if (printer_name == NULL)
		init_vars();
	
	if (run_dialog_print() != GTK_RESPONSE_OK)
		return;
	
	fd = g_file_open_tmp("." PACKAGE "-XXXXXX", &tmp_filename, &error);
	if (error) {
		run_dialog_message(pub->mw->window,
			GTK_MESSAGE_ERROR, error->message);
		g_error_free(error);
		return;
	}
	if (!write_tmp(fd)) {
		basename = get_file_basename(pub->fi->filename, FALSE),
		comline = g_strdup_printf("lpr -P%s -\'#\'%d -T%s %s%s",
			printer_name, print_num, basename,
			print_format ? "-p " : "",
			tmp_filename);
		g_free(basename);
		
//		g_print(">%s\n", comline);
		g_spawn_command_line_sync(comline, &std_out, &std_err, &status, &error);
		g_free(comline);
		
		if (error) {
			run_dialog_message(pub->mw->window,
				GTK_MESSAGE_ERROR, error->message);
			g_error_free(error);
		} else if (status) {
//			g_print("%s%s\n", std_out, std_err);
			if (strlen(std_out))
				run_dialog_message(pub->mw->window,
					GTK_MESSAGE_INFO, "%s",
					g_convert(std_out, -1, "UTF-8", get_default_charset(),
					NULL, NULL, NULL));
			if (strlen(std_err))
				run_dialog_message(pub->mw->window,
					GTK_MESSAGE_ERROR, "%s",
					g_convert(std_err, -1, "UTF-8", get_default_charset(),
					NULL, NULL, NULL));
		}
		g_free(std_out);
		g_free(std_err);
	}
	close(fd);
	
	unlink(tmp_filename);
	g_free(tmp_filename);
}

#endif
