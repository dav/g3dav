// colordialog.c

// Copyright (C) 1999 Dav Coleman

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

// Contact:   Dav Coleman <dav@danger-island.com>
// Homepage:  http://www.danger-island.com/~dav/g3dav/

// this is taken almost completely from chapter 11 of 
// "Developing Linux Applications with GTK+ and GDK"
// by Eric Harlow

#include <gtk/gtk.h>
#include <string.h>

#include "dialogs.h"
    

void CloseDialog( GtkWidget *w, typColorDialogInfo *di ) {
  //g_print( "destroying color selection dialog\n" );
  gtk_grab_remove(w);

  gtk_widget_destroy(w);

  g_free(di->original_color);
  g_free(di);
}


// needs to be specialized
void ColorSelectionChanged( GtkWidget *w, GtkColorSelectionDialog *cs ) {
  GtkColorSelection *colorsel;
  gdouble color[4];

  colorsel = GTK_COLOR_SELECTION( cs->colorsel );
  gtk_color_selection_get_color( colorsel, color );

  g_print( "color changed\n" );
}

//*********************************************************************
void myCancelButtonClicked(GtkWidget *w, typColorDialogInfo *di ) {
  g_print("BUTTON cancel cd.c\n");

  gtk_widget_destroy( di->dialog );
}

//*********************************************************************
void myOkButtonClicked(GtkWidget *w, typColorDialogInfo *di ) {
  GtkColorSelectionDialog *cs;

  g_print("BUTTON ok cd.c\n");
  cs = (GtkColorSelectionDialog*)di->dialog;
  gtk_widget_destroy( GTK_WIDGET(cs) );
}

// ******************************************************************
// this is crashing on re-entry
void GetDialogColor( GdkColor *color, gchar *target, ColorDialogCallbacks* callbacks ) {
  static GtkWidget *window = NULL;
  typColorDialogInfo *di;

  window = gtk_color_selection_dialog_new( target );

  di = g_malloc(sizeof(typColorDialogInfo));
  di->dialog = window;
  di->color = color;
  di->original_color = g_malloc(sizeof(GdkColor));
  memcpy(di->original_color, di->color, sizeof(GdkColor) );

  gtk_color_selection_set_opacity(
    GTK_COLOR_SELECTION( GTK_COLOR_SELECTION_DIALOG( window )->colorsel ), 
    TRUE );

  gtk_color_selection_set_update_policy(
    GTK_COLOR_SELECTION( GTK_COLOR_SELECTION_DIALOG( window )->colorsel ), 
    GTK_UPDATE_CONTINUOUS );

  gtk_signal_connect( GTK_OBJECT( window ), "destroy",
                      GTK_SIGNAL_FUNC( CloseDialog ), di );

  gtk_signal_connect( GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(window)->colorsel), 
                      "color_changed", callbacks->cc_callback, window );

  gtk_signal_connect( GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(window)->ok_button), 
                      "clicked", callbacks->ok_callback, di );

  gtk_signal_connect( GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(window)->cancel_button), 
                      "clicked", callbacks->cancel_callback, di );

  //gtk_signal_connect( GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(window)->ok_button), 
  //                    "clicked", GTK_SIGNAL_FUNC(myOkButtonClicked), di );

  //gtk_signal_connect( GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(window)->cancel_button), 
  //                    "clicked", GTK_SIGNAL_FUNC(myCancelButtonClicked), di );

  gtk_widget_show( window );
  gtk_grab_add( window );

  // this makes it modal in a loop until gtk_main_quit() is called
  //gtk_main();

}


