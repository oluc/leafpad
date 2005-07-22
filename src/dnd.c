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
#include "string.h"

#define DV(x)

static void dnd_open_first_file(gchar *filename)
{
	FileInfo *fi;
	
	if (check_text_modification())
		return;
	fi = g_malloc(sizeof(FileInfo));
	fi->filename = g_strdup(filename);
	fi->charset = pub->fi->charset_flag ? g_strdup(pub->fi->charset) : NULL;
	fi->charset_flag = pub->fi->charset_flag;
	fi->lineend = LF;
	if (file_open_real(pub->mw->view, fi))
		g_free(fi);
	else {
		g_free(pub->fi);
		pub->fi = fi;
		undo_clear_all(pub->mw->buffer);
		set_main_window_title();
//		undo_init(sd->mainwin->textview, sd->mainwin->textbuffer, sd->mainwin->menubar);
	}
}	

static void dnd_drag_data_recieved_handler(GtkWidget *widget,
	GdkDragContext *context, gint x, gint y,
	GtkSelectionData *selection_data, guint info, guint time)
{
	static gboolean flag_called_once = FALSE;
	gchar **files;
	gchar *filename;
	gchar *comline;
	gint i = 0, j = 0;
	gchar *filename_sh;
	gchar **strs;
#ifdef ENABLE_CSDI
	j = 1;
#endif
	
	if (flag_called_once) {
DV(g_print("Drop finished.\n"));
		flag_called_once = FALSE;
		return;
	} else
		flag_called_once = TRUE;
DV({	
	g_print("time                      = %d\n", time);
	g_print("selection_data->selection = %s\n", gdk_atom_name(selection_data->selection));
	g_print("selection_data->target    = %s\n", gdk_atom_name(selection_data->target));
	g_print("selection_data->type      = %s\n", gdk_atom_name(selection_data->type));
	g_print("selection_data->format    = %d\n", selection_data->format);
	g_print("selection_data->data      = %s\n", selection_data->data);
	g_print("selection_data->length    = %d\n", selection_data->length);
});	
	
	if (selection_data->data && g_strstr_len(selection_data->data, 5, "file:")) {
		files = g_strsplit(selection_data->data, "\n" , 0);
		while (files[i]) {
			if (strlen(files[i]) == 0)
				break;
			filename = g_strstrip(parse_file_uri(files[i]));
			if (i + j == 0)
				dnd_open_first_file(filename);
			else {
				if (i + j == 1)
					save_config_file();
				if (strstr(filename, " ")) {
					strs = g_strsplit(filename, " ", -1);
					filename_sh = g_strjoinv("\\ ", strs);
					g_strfreev(strs);
				} else
					filename_sh = g_strdup(filename);
				comline = g_strdup_printf("%s %s", PACKAGE, filename_sh);
DV(g_print(">%s\n", comline));
				g_free(filename_sh);
				g_spawn_command_line_async(comline, NULL);
				g_free(comline);
			}
			g_free(filename);
			i++;
		}
		g_strfreev(files);
	}
}

static GtkTargetEntry drag_types[] =
{
	{ "text/uri-list", 0, 0 }
};

static gint n_drag_types = sizeof(drag_types) / sizeof(drag_types[0]);

void dnd_init(GtkWidget *widget)
{
	gtk_drag_dest_set(widget, GTK_DEST_DEFAULT_ALL,
		drag_types, n_drag_types, GDK_ACTION_COPY);
	g_signal_connect(G_OBJECT(widget), "drag_data_received",
		G_CALLBACK(dnd_drag_data_recieved_handler), NULL);
}
