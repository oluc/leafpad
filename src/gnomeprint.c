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

#ifndef DISABLE_PRINT
#ifdef ENABLE_GNOMEPRINT

#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-config.h>
#include <libgnomeprint/gnome-print-job.h>
#include <libgnomeprintui/gnome-print-dialog.h>
#include <libgnomeprintui/gnome-print-job-preview.h>

GnomePrintJob *create_job(void)
{
	GnomePrintJob *job;
	GnomePrintConfig *gpc;
	GnomePrintContext *gpx;
	GnomeFont *font;
	GnomeFontFace *font_face;
	PangoFontDescription *font_desc;
	gdouble paper_height, margin_left, margin_top, margin_bottom;
	GtkTextIter start, end;
	gchar *text;
	gdouble line_height;
	gchar **lines;
	gint page_top, lines_per_page;
	gint i, current_line = 0;
	
	/* Get values from TextView */
	
	GtkTextBuffer *buffer = pub->mw->buffer;
	gtk_text_buffer_get_start_iter(buffer, &start);
	gtk_text_buffer_get_end_iter(buffer, &end);	
	text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	lines = g_strsplit(text, "\n" , -1);
	g_free(text);
	font_desc = gtk_widget_get_style(pub->mw->view)->font_desc;
	font_face = gnome_font_face_find_closest_from_pango_description(font_desc);
	font = gnome_font_face_get_font_default(font_face,
		pango_font_description_get_size(font_desc) / PANGO_SCALE);
	
	/* Initialize variables */
	
	job = gnome_print_job_new(NULL);
	gpc = gnome_print_job_get_config(job);
	gnome_print_config_get_length(gpc, GNOME_PRINT_KEY_PAPER_HEIGHT,
		&paper_height, NULL);
	gnome_print_config_get_length(gpc, GNOME_PRINT_KEY_PAGE_MARGIN_LEFT,
		&margin_left, NULL);
	gnome_print_config_get_length(gpc, GNOME_PRINT_KEY_PAGE_MARGIN_TOP,
		&margin_top, NULL);
	gnome_print_config_get_length(gpc, GNOME_PRINT_KEY_PAGE_MARGIN_BOTTOM,
		&margin_bottom, NULL);
	page_top = paper_height - margin_top;
	line_height = gnome_font_get_size(font);
	lines_per_page = (paper_height - margin_top - margin_bottom) / line_height;
	if (lines_per_page) // Error check
	
	/* Draw texts to canvas */
	
	do {
		gpx = gnome_print_job_get_context(job);
		gnome_print_beginpage(gpx, NULL);
		gnome_print_setfont(gpx, font);
		for (i = 1; lines[current_line] && (i <= lines_per_page); i++) {
			gnome_print_moveto(gpx, margin_left, page_top - line_height * i);
			gnome_print_show(gpx, lines[current_line++]);
		}
		gnome_print_showpage(gpx);
		g_object_unref(gpx);
	} while (lines[current_line]);
	g_strfreev(lines);
	g_object_unref(gpc);
	g_object_unref(font);
	gnome_print_job_close(job);
	
	return job;
}

gint create_gnomeprint_session(void)
{
	GnomePrintJob *job = NULL;
	GtkWidget *dialog, *preview;
	gint res;
	
	dialog = gnome_print_dialog_new(NULL, _("Print"), 0);
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(pub->mw->window));
	do {
		res = gtk_dialog_run(GTK_DIALOG(dialog));
		switch (res) {
		case GNOME_PRINT_DIALOG_RESPONSE_PRINT:
			if (!job)
				job = create_job();
			gnome_print_job_print(job);
			break;
		case GNOME_PRINT_DIALOG_RESPONSE_PREVIEW:
			if (!job)
				job = create_job();
			preview = gnome_print_job_preview_new(job, _("Print Preview"));
//			gtk_window_set_transient_for(GTK_WINDOW(preview), GTK_WINDOW(dialog));
			gtk_window_set_modal(GTK_WINDOW(preview), TRUE);
			gtk_widget_show(preview);
			g_signal_connect(G_OBJECT(preview), "destroy",
				G_CALLBACK(gtk_main_quit), NULL);
			gtk_main();
			break;
		}
	} while (res == GNOME_PRINT_DIALOG_RESPONSE_PREVIEW);
	gtk_widget_destroy(dialog);
	if (job)
		g_object_unref(job);
	
	return res;
}

#endif
#endif
