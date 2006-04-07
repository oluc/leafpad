/*
 *  Leafpad - GTK+ based simple text editor
 *  Copyright (C) 2004-2006 Tarot Osuji
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
 
#ifdef ENABLE_EMACS

#include "leafpad.h"
#include <gdk/gdkkeysyms.h>

static void emacs_key_prefix(void)
{
	/* waiting for release C-x */
	;
	
	/* waiting for input sequence */
	;
}

gboolean check_emacs_key_theme(GtkWindow *window, GtkItemFactory *ifactory)
{
	GtkAccelGroup *accel_group;
	GSList *groups;
	gchar *key_theme = NULL;
	gboolean emacs_flag = FALSE;
	GtkSettings *settings = gtk_settings_get_default();
	
	g_object_get(settings, "gtk-key-theme-name", &key_theme, NULL);
	if (key_theme) {
		if (!g_ascii_strcasecmp(key_theme, "Emacs"))
			emacs_flag = TRUE;
		g_free(key_theme);
	}
	if (!emacs_flag)
		return FALSE;
	
	groups = gtk_accel_groups_from_object(G_OBJECT(window));
	accel_group = groups->data;
	if (accel_group) {
		gtk_window_remove_accel_group(GTK_WINDOW(window), accel_group);
		g_object_unref(accel_group);
	}
	accel_group = gtk_accel_group_new();
	
/*	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/File/New"),
		accel_group, GDK_N, GDK_CONTROL_MASK);
	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/File/Open..."),
		accel_group, GDK_O, GDK_CONTROL_MASK);
	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/File/Save"),
		accel_group, GDK_S, GDK_CONTROL_MASK);
	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/File/Save As..."),
		accel_group, GDK_S, GDK_SHIFT_MASK | GDK_CONTROL_MASK);
#ifdef ENABLE_PRINT
	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/File/Print..."),
		accel_group, GDK_P, GDK_CONTROL_MASK);
#endif
	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Edit/Undo"),
		accel_group, GDK_Z, GDK_CONTROL_MASK);
*/	gtk_widget_add_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Edit/Undo"),
		"activate", accel_group, GDK_underscore, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
/*	gtk_widget_add_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Edit/Undo"),
		"activate", accel_group, GDK_slash, GDK_CONTROL_MASK, 0);
	gtk_widget_add_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Edit/Redo"),
		"activate", accel_group, GDK_backslash, GDK_CONTROL_MASK, 0);
*/	gtk_accel_group_connect(
		accel_group, GDK_backslash, GDK_CONTROL_MASK, 0,
		g_cclosure_new_swap(G_CALLBACK(on_edit_redo), NULL, NULL));
/*	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Edit/Select All"),
		accel_group, GDK_A, GDK_CONTROL_MASK);
	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Search/Find..."),
		accel_group, GDK_F, GDK_CONTROL_MASK);
	gtk_widget_add_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Search/Find..."),
		"activate", accel_group, GDK_S, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
*/	gtk_widget_add_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Search/Find Next"),
		"activate", accel_group, GDK_S, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
	gtk_widget_add_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Search/Find Previous"),
		"activate", accel_group, GDK_R, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
/*	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Search/Replace..."),
		accel_group, GDK_H, GDK_CONTROL_MASK);
*/	gtk_widget_add_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Search/Replace..."),
		"activate", accel_group, GDK_percent, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
/*	gtk_widget_remove_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Search/Jump To..."),
		accel_group, GDK_J, GDK_CONTROL_MASK);
*/	gtk_widget_add_accelerator(
		gtk_item_factory_get_widget(ifactory, "/Search/Jump To..."),
		"activate", accel_group, GDK_G, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
	
	gtk_accel_group_connect(
		accel_group, GDK_X, GDK_CONTROL_MASK, 0,
		g_cclosure_new_swap(G_CALLBACK(emacs_key_prefix), NULL, NULL));
	
	gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
	
	return TRUE;
}

#endif
