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

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include "dialogs.h"

GdkGC* penRed;
GdkGC* penCyan;
GdkColor *ColorRed;
GdkColor *ColorCyan;

GdkGC* working_penRed;
GdkGC* working_penCyan;
GdkColor *working_ColorRed;
GdkColor *working_ColorCyan;

GdkGC* penAdj;
GdkColor *ColorAdj;
GdkGC* penOriginal;
GdkColor *ColorOriginal;

GtkWidget *canvas;
GdkPixmap *offscreenBuffer = NULL;

static gchar* doing_red = "red";
static gchar* doing_cyan = "cyan";

#define DOING_RED    1
#define DOING_CYAN   2

// I hate using externs, I need to fix this soon
extern void GetDialogColor( GdkColor *color, gchar *target, ColorDialogCallbacks* callbacks );


gchar* ana_info =
  "The mathematical formula which\n"
  "translates a 3-D point into\n"
  "dual 2-D points depends on two\n"
  "values:\n\n"
  "The distance between your eyes\n"
  "and the distance from your\n"
  "eyes to the screen.\n\n"
  "Adjust to taste.";

gchar* color_info =
  "Put on your anaglyphic glasses\n"
  "then close one eye at a time\n"
  "and ensure that each eye sees\n"
  "only one line above by using\n"
  "the color adjustment buttons";



//*********************************************************************
void message_dialog( gchar* message ) {
  GtkWidget *dlg=NULL;
  GtkWidget *button=NULL;
  GtkWidget *label=NULL;

  dlg = gtk_dialog_new();

  gtk_window_set_title( GTK_WINDOW(dlg), "Message:" );

  gtk_container_border_width( GTK_CONTAINER(dlg), 3 );

  label = gtk_label_new( message );
  gtk_misc_set_padding( GTK_MISC(label), 7, 7 );

  button = gtk_button_new_with_label( "OK" );
  GTK_WIDGET_SET_FLAGS( button, GTK_CAN_DEFAULT );

  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), label, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->action_area ), button, TRUE, TRUE, 0 );

  gtk_widget_show(label);
  gtk_widget_show(button);

  gtk_widget_grab_default( button );

  gtk_signal_connect( GTK_OBJECT( dlg ), "destroy", GTK_SIGNAL_FUNC( destroy_dialog ), dlg );
  gtk_signal_connect( GTK_OBJECT( button ), "clicked", GTK_SIGNAL_FUNC( close_dialog ), dlg );

  gtk_widget_show( dlg );

  gtk_grab_add( dlg );
}

//*********************************************************************
// called by clicking OK button
// closes the message dialog
void close_dialog( GtkWidget *widget, gpointer data ) {
  gtk_widget_destroy( GTK_WIDGET( data ) );
}

//*********************************************************************
void parameter_adjust_dialog( g3dav_parameters* p ) {
  static GtkWidget *dlg=NULL;
  GtkWidget *btnOK=NULL;
  GtkWidget *btnCancel=NULL;
  GtkWidget *lblView=NULL;
  GtkWidget *lblEye=NULL;
  GtkWidget *frmInfo=NULL;
  GtkWidget *taInfo=NULL;
  static GtkWidget *txtView=NULL;
  static GtkWidget *txtEye=NULL;
  static param_update_pkg* info;

  char buffer[80];


  dlg = gtk_dialog_new();

  gtk_window_set_title( GTK_WINDOW(dlg), "Adjust Parameters" );

  gtk_container_border_width( GTK_CONTAINER(dlg), 3 );

  lblEye = gtk_label_new( "Eye Width:" );

  lblView = gtk_label_new( "View Distance:" );


  txtEye = gtk_entry_new( );
  txtView = gtk_entry_new( );             

  frmInfo = gtk_frame_new("Anaglyphic Parameters" );
  taInfo = gtk_text_new( NULL, NULL );
  gtk_text_insert( GTK_TEXT( taInfo ), NULL, NULL, NULL, ana_info, strlen( ana_info ) );
  gtk_text_set_editable( GTK_TEXT( taInfo ), FALSE );

  gtk_container_add( GTK_CONTAINER( frmInfo ), taInfo );

  btnOK = gtk_button_new_with_label( "OK" );
  btnCancel = gtk_button_new_with_label( "Cancel" );

  //gtk_misc_set_padding( GTK_MISC(label), 7, 7 );
  sprintf( buffer, "%f", *(p->eye_dis) );
  gtk_entry_set_text( GTK_ENTRY(txtEye), buffer );
  sprintf( buffer, "%f", *(p->view_dis) );
  gtk_entry_set_text( GTK_ENTRY(txtView), buffer );

  info = g_malloc( sizeof(param_update_pkg) );
  info->p = p;
  info->eye = GTK_ENTRY(txtEye);
  info->view = GTK_ENTRY(txtView);
  info->dialog = dlg;
  
  GTK_WIDGET_SET_FLAGS( txtEye, GTK_CAN_DEFAULT );

  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), lblEye, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), txtEye, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), lblView, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), txtView, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), frmInfo, TRUE, TRUE, 0 );

  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->action_area ), btnOK, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->action_area ), btnCancel, TRUE, TRUE, 0 );

  gtk_widget_show(btnOK);
  gtk_widget_show(btnCancel);
  gtk_widget_show(lblView);
  gtk_widget_show(lblEye);
  gtk_widget_show(txtView);
  gtk_widget_show(txtEye);
  gtk_widget_show(frmInfo);
  gtk_widget_show(taInfo);

  gtk_widget_grab_default( txtEye );

  gtk_signal_connect( GTK_OBJECT( dlg ), "destroy", 
                      GTK_SIGNAL_FUNC( destroy_param_dialog ), info );

  gtk_signal_connect( GTK_OBJECT( btnOK ), "clicked", 
                      GTK_SIGNAL_FUNC( update_parameters ), info );

  gtk_signal_connect( GTK_OBJECT( btnCancel ), "clicked", 
                      GTK_SIGNAL_FUNC( cancel_dialog ), dlg );

  gtk_widget_show( dlg );

  gtk_grab_add( dlg );

} // end parameter_adjust_dialog

//*********************************************************************
// called by clicking OK button
// closes the paramter update dialog
void update_parameters( GtkWidget *widget, gpointer data ) {
  //float f;
  gchar* text;
  param_update_pkg* info = (param_update_pkg*)data;
  
  text = gtk_entry_get_text( info->eye );
  *(info->p->eye_dis) = atof( text );
  text = gtk_entry_get_text( info->view );
  *(info->p->view_dis) = atof( text );

  gtk_widget_destroy( info->dialog ); // remove & send destroy signal
}

//*********************************************************************
gint destroy_param_dialog( GtkWidget *widget, gpointer data ) {

  param_update_pkg* info = (param_update_pkg*)data;

  gtk_grab_remove( GTK_WIDGET( widget ) );


  //g_print( "destroy 0 : info->p @ %ld\n", (glong)info->p );
  //g_free( info->p );
  //info->p=NULL;
  g_free( info );

  return (FALSE);
}


//*********************************************************************
// called by clicking OK button
// closes the g3dav color adjustment dialog
// also closes the message dialog
void close_g3dav_color_dialog( GtkWidget *widget, gpointer data ) {

  // Make sure color changes update main view colors
  memcpy(ColorRed, working_ColorRed, sizeof(GdkColor) );
  memcpy(ColorCyan, working_ColorCyan, sizeof(GdkColor) );

  gdk_gc_set_foreground( penRed, ColorRed );
  gdk_gc_set_foreground( penCyan, ColorCyan );

  //g_print( "ColorRed            : (r %x) (g %x) (b %x)\n\n", ColorRed->red,ColorRed->green,ColorRed->blue );

  gtk_widget_destroy( GTK_WIDGET( data ) );

  // NEED TO DO : update main view colors at this point
}


//*********************************************************************
void cancel_dialog( GtkWidget *widget, gpointer data ) {
  gtk_widget_destroy( GTK_WIDGET( data ) );
}

//*********************************************************************
gint destroy_dialog( GtkWidget *widget, gpointer data ) {
  if(working_penRed) {
    gdk_gc_destroy(working_penRed);
    working_penRed=NULL;
  }
  if(working_penCyan) {
    gdk_gc_destroy(working_penCyan);
    working_penCyan = NULL;
  }
  if(working_ColorRed) {
    g_free(working_ColorRed);
    working_ColorRed = NULL;
  }
  if(working_ColorCyan) {
    g_free(working_ColorCyan);
    working_ColorCyan = NULL;
  }

  gtk_grab_remove( GTK_WIDGET( widget ) );

  return (FALSE);
}

//*********************************************************************
// dialog which allows user to choose either red or cyan adjustment
//  - draws two lines for calibration
void g3dav_color_dialog( GdkGC* penR, GdkGC* penC, 
                         GdkColor *ColorR, GdkColor *ColorC ) {
  GtkWidget *dlg=NULL;
  GtkWidget *btnRed=NULL, *btnCyan=NULL;
  GtkWidget *btnOK=NULL,  *btnCancel=NULL;
  GtkWidget *label=NULL;
  GtkWidget *frmInfo=NULL;
  GtkWidget *taInfo=NULL;

  penRed = penR;
  penCyan = penC;
  ColorRed = ColorR;
  ColorCyan = ColorC;


  // get our working pens/colors
  working_ColorRed = g_malloc( sizeof(GdkColor) );
  memcpy(working_ColorRed, ColorR, sizeof(GdkColor) );
  working_ColorCyan = g_malloc( sizeof(GdkColor) );
  memcpy(working_ColorCyan, ColorC, sizeof(GdkColor) );


  dlg = gtk_dialog_new();


  gtk_window_set_title( GTK_WINDOW(dlg), "Color Adjustment" );

  gtk_container_border_width( GTK_CONTAINER(dlg), 3 );

  canvas = gtk_drawing_area_new();
  // Listen only for these events (best to set this ASAP, before widget is realized)
  //gtk_widget_set_events( canvas, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
  //                       GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK );

  gtk_drawing_area_size( GTK_DRAWING_AREA(canvas), 100, 100 );

  //offscreenBuffer = gdk_pixmap_new( (GdkDrawable*)(canvas->window), 100, 100, -1 );

  label = gtk_label_new( "Adjust colors" );
  gtk_misc_set_padding( GTK_MISC(label), 7, 7 );

  btnRed = gtk_button_new_with_label( "Red" );
  btnCyan = gtk_button_new_with_label( "Cyan" );
  btnOK = gtk_button_new_with_label( "OK" );
  btnCancel = gtk_button_new_with_label( "Cancel" );

  frmInfo = gtk_frame_new("Color Adjustment" );
  taInfo = gtk_text_new( NULL, NULL );
  gtk_text_insert( GTK_TEXT( taInfo ), NULL, NULL, NULL, color_info, strlen( color_info ) );
  gtk_text_set_editable( GTK_TEXT( taInfo ), FALSE );

  gtk_container_add( GTK_CONTAINER( frmInfo ), taInfo );


  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), label, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), canvas, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->vbox ), frmInfo, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->action_area ), btnRed, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->action_area ), btnCyan, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->action_area ), btnOK, TRUE, TRUE, 0 );
  gtk_box_pack_start( GTK_BOX( GTK_DIALOG( dlg )->action_area ), btnCancel, TRUE, TRUE, 0 );

  gtk_widget_show(label);
  gtk_widget_show(canvas);
  gtk_widget_show(frmInfo);
  gtk_widget_show(taInfo);
  gtk_widget_show(btnRed);
  gtk_widget_show(btnCyan);
  gtk_widget_show(btnOK);
  gtk_widget_show(btnCancel);

  //gtk_widget_grab_default( button );

  gtk_signal_connect( GTK_OBJECT( dlg ), "destroy", 
                      GTK_SIGNAL_FUNC( destroy_dialog ), dlg );

  gtk_signal_connect( GTK_OBJECT( btnOK ), "clicked", 
                      GTK_SIGNAL_FUNC( close_g3dav_color_dialog ), dlg );

  gtk_signal_connect( GTK_OBJECT( btnCancel ), "clicked", 
                      GTK_SIGNAL_FUNC( cancel_dialog ), dlg );

  gtk_signal_connect( GTK_OBJECT( btnRed ), "clicked", 
                      GTK_SIGNAL_FUNC( do_color_select ), doing_red );

  gtk_signal_connect( GTK_OBJECT( btnCyan ), "clicked", 
                      GTK_SIGNAL_FUNC( do_color_select ), doing_cyan );

  gtk_signal_connect( GTK_OBJECT(canvas), "expose_event", 
                      GTK_SIGNAL_FUNC(g3dav_color_dlg_expose_event), NULL );

  gtk_signal_connect( GTK_OBJECT(canvas), "configure_event", 
                      GTK_SIGNAL_FUNC(g3dav_color_dlg_configure_event), NULL );

  g_assert( dlg!=NULL );
  gtk_widget_show( dlg );

  gtk_grab_add( dlg );


}


//*********************************************************************
// this event handler should be called whenever the drawable is created
// or resized
gint g3dav_color_dlg_configure_event( GtkWidget *widget, GdkEventConfigure *event) {
  // this method assumes the widget is the canvas

  //g_print( "configure\n" );
  
  repaint_color_dlg_canvas(widget);

  return(TRUE);
}

//*********************************************************************
// this event handler should be called whenever the window is exposed
// to the viewer or the gdk_widget_draw routine is called.
gint g3dav_color_dlg_expose_event( GtkWidget *widget, GdkEventExpose *event) {
  // copy the offscreen pixmap to the window
  
  //g_print( "expose\n" );

  if (widget!=NULL && offscreenBuffer!=NULL) {
    gdk_draw_pixmap( widget->window,
                     widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                     offscreenBuffer,
                     event->area.x, event->area.y,
                     event->area.x, event->area.y,
                     event->area.width, event->area.height );
  }

  return(FALSE);
}

//*********************************************************************
gint repaint_color_dlg_canvas( gpointer data ) {
  GtkWidget* drawing_area = (GtkWidget*)data;
  GdkDrawable *drawable;
  guint width, height;
  GdkRectangle update_rect;
  gint rx, cx;


  //g_print( "repaint\n" );

  //GdkFont* font = gdk_font_load("fixed");
  drawing_area->style->font = gdk_font_load("fixed");

  width = drawing_area->allocation.width;
  height = drawing_area->allocation.height;

  drawable = drawing_area->window;
  g_assert(drawable!=NULL);

  if (!offscreenBuffer) {
    offscreenBuffer = gdk_pixmap_new( drawable, width, height, -1 );
  }

  if (working_penRed==NULL) {
    working_penRed = gdk_gc_new(offscreenBuffer);
    gdk_gc_set_foreground( working_penRed, working_ColorRed );
  }

  if (working_penCyan==NULL) {
    working_penCyan = gdk_gc_new(offscreenBuffer);
    gdk_gc_set_foreground( working_penCyan, working_ColorCyan );
  }

  // clear the offscreen buffer
  gdk_draw_rectangle( offscreenBuffer, drawing_area->style->black_gc, 
                      TRUE, 0, 0,width,height );

  rx = (gint)(width*0.33);
  if (working_penRed!=NULL) {
    gdk_draw_line( offscreenBuffer, (GdkGC*)working_penRed, rx, 0, rx, height );
  }


  cx = (gint)(width*0.66);
  if (working_penCyan!=NULL) {
    gdk_draw_line( offscreenBuffer, (GdkGC*)working_penCyan, cx, 0, cx, height );
  }

  // Now draw the canvas to the screen
  update_rect.x=0;
  update_rect.y=0;
  update_rect.width=drawing_area->allocation.width;
  update_rect.height=drawing_area->allocation.height;
  // this causes an expose_event signal
  gtk_widget_draw( drawing_area, &update_rect );

  return(TRUE);
  
}

//*********************************************************************
// needs to be specialized
void ColorChanged( GtkWidget *w, GtkColorSelectionDialog *cs ) {
  GtkColorSelection *colorsel;
  gdouble color[4];

  colorsel = GTK_COLOR_SELECTION( cs->colorsel );
  gtk_color_selection_get_color( colorsel, color );

  g_assert( ColorAdj!=NULL);

  ColorAdj->red  = color[0] * 0xffff;
  ColorAdj->green= color[1] * 0xffff;
  ColorAdj->blue = color[2] * 0xffff;

  //g_print( "dialog color changed: (r %x) (g %x) (b %x)\n", ColorAdj->red,ColorAdj->green,ColorAdj->blue );
  //g_print( "penr=Red            : (r %x) (g %x) (b %x)\n\n", ColorRed->red,ColorRed->green,ColorRed->blue );

  g_assert(penAdj!=NULL);

  // perhaps I should dealloc the old color? need to look that up
  gdk_color_alloc( gdk_colormap_get_system(), ColorAdj );
  gdk_gc_set_foreground( penAdj, ColorAdj );

  repaint_color_dlg_canvas(canvas);

}

//*********************************************************************
void CancelButtonClicked(GtkWidget *w, typColorDialogInfo *di ) {
  // don't save any color changes!

  //g_print("BUTTON cancel in dialog.c\n");

  //gtk_signal_emit_by_name( GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(di->dialog)->colorsel),
  //                          "color_changed", (GtkColorSelectionDialog*)di->dialog );


  // this resets working_ColorX to its original
  memcpy( ColorAdj, ColorOriginal, sizeof(GdkColor) );
  // perhaps I should dealloc the old color? need to look that up
  gdk_color_alloc( gdk_colormap_get_system(), ColorAdj );
  gdk_gc_set_foreground( penAdj, ColorAdj );


  repaint_color_dlg_canvas(canvas);

  /*
  
  these g_free calls were causing subsequent calls to 
  gtk_color_selection_dialog_new( target ) to crash
  
  if (ColorOriginal) { g_free(ColorOriginal); }
  if (penOriginal) { g_free(penOriginal); }
  */

  gtk_widget_destroy( di->dialog );
}

//*********************************************************************
// Ok Button on GtkSelectionDialog
void OkButtonClicked(GtkWidget *w, typColorDialogInfo *di ) {

  // actually, we probably don't have to do  this because
  // should have already been called by color_signal event
  ColorChanged( w,  (GtkColorSelectionDialog*)di->dialog );

  /*
  see Cancel function 
  if (ColorOriginal) { g_free(ColorOriginal); }
  if (penOriginal) { g_free(penOriginal); }
  */

  gtk_widget_destroy( GTK_WIDGET((GtkColorSelectionDialog*)di->dialog) );
}



//*********************************************************************
void do_color_select( GtkWidget* w, gpointer* data ) {
  gchar* which;
  static ColorDialogCallbacks callbacks;

  g_assert(data!=NULL);

  if (!ColorOriginal) {
    ColorOriginal = g_malloc(sizeof(GdkColor));
  }
  if (!penOriginal) {
    penOriginal = g_malloc(sizeof(GdkGC));
  }

  which = (gchar*)data;

  if (which == doing_red) {
    ColorAdj = working_ColorRed;
    penAdj = working_penRed;
    which = "Adjust cyan color\n";
  } else if (which == doing_cyan) {
    ColorAdj = working_ColorCyan;
    penAdj = working_penCyan;
    which = "Adjust red color\n";
  } else {
    g_assert_not_reached();
    return;
  }
  
  g_assert(ColorOriginal!=NULL);
  g_assert(penOriginal!=NULL);

  // save original color in case user cancels
  memcpy(ColorOriginal,ColorAdj,sizeof(GdkColor));
  memcpy(penOriginal,penAdj,sizeof(GdkGC));

  callbacks.cc_callback = GTK_SIGNAL_FUNC(ColorChanged);
  callbacks.cancel_callback = GTK_SIGNAL_FUNC(CancelButtonClicked);
  callbacks.ok_callback = GTK_SIGNAL_FUNC(OkButtonClicked);

  GetDialogColor( ColorAdj, which, &callbacks );

}
