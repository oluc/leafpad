/*
 *  dialog.c
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
#include "intl.h"
#include "dialog.h"

/*	GTK_MESSAGE_INFO, GTK_MESSAGE_WARNING, GTK_MESSAGE_ERROR */
void run_dialog_message(GtkWidget *window, GtkMessageType type, gchar *message, ...)
{
	va_list ap;
	GtkWidget *dialog;
	gchar *str;
	
	va_start(ap, message);
		str = g_strdup_vprintf(message, ap);
	va_end(ap);
	
	dialog = gtk_message_dialog_new(GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		type,
		GTK_BUTTONS_OK,
		str);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	g_free(str);
	
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

GtkWidget *create_dialog_message_question(GtkWidget *window, gchar *message, ...)
{
	va_list ap;
	GtkWidget *dialog;
	gchar *str;
	
	va_start(ap, message);
		str = g_strdup_vprintf(message, ap);
	va_end(ap);
	
	dialog = gtk_message_dialog_new(GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_QUESTION,
		GTK_BUTTONS_NONE,
		str);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	gtk_dialog_add_buttons (GTK_DIALOG (dialog),
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_NO, GTK_RESPONSE_NO,
		GTK_STOCK_YES, GTK_RESPONSE_YES,
		NULL); 
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_YES);
	g_free(str);
	
	return dialog;
}

gint run_dialog_message_question(GtkWidget *window, gchar *message, ...)
{
	va_list ap;
	GtkWidget *dialog;
	gchar *str;
	gint res;
	
	va_start(ap, message);
		str = g_strdup_vprintf(message, ap);
	va_end(ap);
	
	dialog = create_dialog_message_question(window, str);
	g_free(str);
	
	res = gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
	
	return res;
}

void run_dialog_about(GtkWidget *window, const gchar *name, const gchar *version,
	const gchar *description, const gchar *copyright, gchar *iconpath)
{
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *icon;
	GtkWidget *margin;
	GtkWidget *label;
	gchar *str;
	
	const gchar *env;
	gchar *translator = NULL;
	gchar *translation;
	
	dialog = gtk_dialog_new_with_buttons(
		_("About"),
		GTK_WINDOW(window),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_OK, GTK_RESPONSE_CANCEL,
		NULL);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), vbox, FALSE, FALSE, 0);
	
	icon = gtk_image_new_from_file(iconpath);
	gtk_box_pack_start (GTK_BOX (vbox), icon, FALSE, FALSE, 0);
	
	margin = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(margin), 4);
	gtk_box_pack_start(GTK_BOX(vbox), margin, FALSE, FALSE, 0);
	
	/* TODO: use array */
	env = g_getenv("LC_ALL");
	if (!env)
		env = g_getenv("LANG");
	if (env) {
		if (strncmp(env, "cs", 2) == 0)
			translator = "2004 Petr Vyslou&#382;il";
		if (strncmp(env, "de", 2) == 0)
			translator = "2004 Sebastian Stach";
		if (strncmp(env, "es", 2) == 0)
			translator = "2004 Lucas Vieites";
		if (strncmp(env, "fr", 2) == 0)
			translator = "2004 Luc Pionchon";
		if (strncmp(env, "ru", 2) == 0)
			translator = "2004 Artem Vakhitov";
		if (strncmp(env, "sv", 5) == 0)
			translator = "2004 Isak Savo";
		if (strncmp(env, "zh_CN", 5) == 0)
			translator = "2004 Carlos Z.F. Liu";
		if (strncmp(env, "zh_TW", 5) == 0)
			translator = "2004 OLS3";
	}
	if (translator)
		translation = g_strdup_printf("\n<small>Translation &#169; %s</small>", translator);
//		translation = g_strdup_printf("\n\n<small>Translated by\n%s</small>", translator);
	else
		translation = g_strdup("");
 	
	label = gtk_label_new(NULL);
	str = g_strdup_printf(
		"<span size=\"xx-large\" weight=\"heavy\">%s %s</span>\n<span size=\"xx-small\">\n</span>%s\n<span size=\"xx-small\">\n</span><small>%s</small>%s",
		name, version, description, copyright, translation);
	gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
	gtk_label_set_markup(GTK_LABEL(label), str);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	g_free(str);
	g_free(translation);
	
	gtk_widget_show_all(vbox);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
