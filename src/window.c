/*
 *  window.c
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

static gboolean cb_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	cb_file_quit(data);
	
	return TRUE;
}
/*
static void cb_scroll_event(GtkAdjustment *adj, GtkWidget *view)
{
	gtk_text_view_place_cursor_onscreen(GTK_TEXT_VIEW(view));
}
*/
MainWindow *create_main_window(StructData *sd)
{
	GtkWidget *window;
	GtkWidget *vbox;
 	GtkWidget *menubar;
 	GtkWidget *sw;
 	GtkWidget *textview;
	GtkTextBuffer *textbuffer;
 	GdkPixbuf *icon;
	
	MainWindow *mainwin = g_malloc(sizeof(MainWindow));
	
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(GTK_WINDOW(window), sd->conf.width, sd->conf.height);
	gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME);
	icon = gdk_pixbuf_new_from_file(ICONDIR G_DIR_SEPARATOR_S PACKAGE ".png", NULL);
	gtk_window_set_icon(GTK_WINDOW(window), icon);
	g_signal_connect(G_OBJECT(window), "delete-event",
		G_CALLBACK(cb_delete_event), sd);
	
	vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	menubar = create_menu_bar(window, sd);
	gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
	
	sw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(vbox), sw, TRUE, TRUE, 0);
	
	textview = gtk_text_view_new();
	gtk_container_add(GTK_CONTAINER(sw), textview);
	
	textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
/*	//Following code has possibility of confliction if scroll policy of GTK changed
	GtkAdjustment *hadj = gtk_scrolled_window_get_hadjustment(GTK_SCROLLED_WINDOW(sw));
	GtkAdjustment *vadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(sw));
	
	g_signal_connect_after(G_OBJECT(hadj), "value-changed",
		G_CALLBACK(cb_scroll_event), textview);
	g_signal_connect_after(G_OBJECT(vadj), "value-changed",
		G_CALLBACK(cb_scroll_event), textview);
*/	
	mainwin->window = window;
	mainwin->menubar = menubar;
	mainwin->textview = textview;
	mainwin->textbuffer = textbuffer;
	
	return mainwin;
}

gchar *get_current_file_basename(gchar *filepath)
{
	gchar *basename;
	
	if (filepath)
		basename = g_path_get_basename(g_filename_to_utf8(filepath, -1, NULL, NULL, NULL));
	else
		basename = g_strdup(_("Untitled"));
	
	return basename;
}

void set_main_window_title(StructData *sd)
{
	gchar *basename, *title;
	
	basename = get_current_file_basename(sd->fi->filepath);
	if (sd->fi->filepath) {
		if (g_file_test(g_filename_to_utf8(sd->fi->filepath, -1, NULL, NULL, NULL),
			G_FILE_TEST_EXISTS))
			title = g_strdup(basename);
		else
			title = g_strdup_printf("(%s)", basename);
	} else
		title = g_strdup_printf("(%s)", basename);
//		title = g_strdup(basename);
//		title = g_strdup_printf(PACKAGE_NAME);
	gtk_window_set_title(GTK_WINDOW(sd->mainwin->window), title);
	g_free(title);
	g_free(basename);
}
