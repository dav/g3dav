// g3dav GNU 3-Dimensional Anaglyphic Viewer

// A GTK+ application which will display an anaglyphic version
// of a 3-Dimensional object
//
// This code is a port of a Java application I wrote back when 
// the JDK 1.0 came out. The Java application was a port of a
// DOS Turbo C++ application I wrote back around 1990.

// Both ports represent my first foray into a new development
// language/platform.

// This code will get a major overhaul before the next release.
// It will be altered to use the Gnome UI Library, and may also
// be rewritten in GTK-- (C++ Wrapper for GTK+) because frankly
// object oriented code is much more elegant in GUI application
// design (to me)

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
#include <stdlib.h>
#include <math.h>

#include "model3d.h"
#include "matrix3d.h"
#include "anaglyphic.h"
#include "dialogs.h"
#include "g3dav.h"

#define VERSION "0.0.4"

// GLOBALS 
//   Most of these won't be global within a few releases.
static GdkPixmap *offscreenBuffer = NULL;
static GdkGC* penRed=NULL;
static GdkGC* penCyan=NULL;
static GdkColor *ColorRed = NULL;
static GdkColor *ColorCyan = NULL;

GtkWidget*          window;
GtkWidget*          canvas3d;             

GList *models=NULL;
GList *matrices=NULL;
GList *CurrentModel=NULL;
GList *CurrentMatrix=NULL;

g3dav_model*        model = NULL;      //pointer to the current model structure
g3dav_anaglyphic*   anaglyphic = NULL; //pointer to current anaglyphic
g3dav_matrix*       matrix = NULL;
anaglyphic_line*    ana_line;          // used by repaint

gint                Swidth, Sheight;  // need to consolidate various width/height
                                      // variables to use these.

gint                prev_mouse1x, prev_mouse1y;
gint                prev_mouse3x, prev_mouse3y;
// following used also for y since our world is square, man
gdouble             prev_wxmin, prev_wxmax; 
static g3dav_parameters*   parameters;

// defines for functions used in menu table below
static void LoadModel( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void ShowNext( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void ShowPrev( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void SquareWindow( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void ToggleRAxis( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void ToggleOAxis( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void CloseModel( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void ColorAdjust( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void ParamAdjust( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void ZoomIn(gpointer callback_data, guint callback_action, GtkWidget *widget);
static void ZoomOut( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void DefaultView( gpointer callback_data, guint callback_action, GtkWidget *widget);
static void NotReady( gpointer callback_data, guint callback_action, GtkWidget *widget);


// menu definitions
static GtkItemFactoryEntry menu_items[] = {
   { "/_File",                0,                   0,             0,          "<Branch>"},
   { "/File/_Open",           "<control>o",        LoadModel,     0,          "<Item>"},
   { "/File/_Close",          "F3",                CloseModel,    0,          "<Item>"},
   { "/File/_Save",           "<control>s",        NotReady,      0,          "<Item>"},
   { "/_Options",             "<alt>o",            0,             0,          "<Branch>"},
   { "/Options/Adjust _Colors",  "<control>c",     ColorAdjust,   0,          "<Item>"},
   { "/Options/Adjust _Anaglyphics",  "<control>v",ParamAdjust,   0,          "<Item>"},
   { "/_View",                "<alt>v",            0,             0,          "<Branch>"},
   { "/View/Rotational A_xes","<control>x",        ToggleRAxis,   0,          "<ToggleItem>"},
   { "/View/Ori_gin Axes",    "<control>x",        ToggleOAxis,   0,          "<ToggleItem>"},
   { "/View/mnsep1",          NULL,                NULL,          0,          "<Separator>"},
   { "/View/_Default",        "<control>v",        DefaultView,   0,          "<Item>"},
   { "/View/Zoom _In",         "=",                ZoomIn,        0,          "<Item>"},
   { "/View/Zoom _Out",        "-",                ZoomOut,       0,          "<Item>"},
   { "/_Window",              "<alt>w",            0,             0,          "<Branch>"},
   { "/Window/_Previous",     "<control>p",        ShowPrev,      0,          "<Item>"},
   { "/Window/_Next",         "<control>n",        ShowNext,      0,          "<Item>"},
   { "/View/mnsep1",          NULL,                NULL,          0,          "<Separator>"},
   { "/Window/_Square",       "<control>q",        SquareWindow,  0,          "<Item>"}
};


//*********************************************************************
//*********************************************************************
// Code starts here
//*********************************************************************
//*********************************************************************

//*********************************************************************
// a callback for delete_event
gint AnyLastRequests( GtkWidget *widget, gpointer *data ) {
   // nope, go ahead and kill me

   // NEED TO DO: 
   //  check with user, save changes (models, configuration)?

   gtk_main_quit();
   return(FALSE);
}

/*
 * Create application toolbar
 */
void CreateToolbar (GtkWidget *vbox_main) {
   GtkWidget *toolbar;


   /* --- Create the toolbar and add it to the window --- */
   toolbar = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
   gtk_box_pack_start (GTK_BOX (vbox_main), toolbar, FALSE, TRUE, 0);
   gtk_widget_show (toolbar);

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Open 3d Object", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_open),
                            (GtkSignalFunc) LoadModel,
                            NULL);

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Save 3d Object", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_save),
                            (GtkSignalFunc) NotReady,
                            NULL);

   gtk_toolbar_append_space (GTK_TOOLBAR (toolbar));

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Adjust Colors", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_rgb),
                            (GtkSignalFunc) ColorAdjust,
                            NULL);

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Adjust Anaglyphics", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_3d),
                            (GtkSignalFunc) ParamAdjust,
                            NULL);

   gtk_toolbar_append_space (GTK_TOOLBAR (toolbar));

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Previous", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_prev),
                            (GtkSignalFunc) ShowPrev,
                            NULL);

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Next", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_next),
                            (GtkSignalFunc) ShowNext,
                            NULL);

   gtk_toolbar_append_space (GTK_TOOLBAR (toolbar));

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Square Window", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_square),
                            (GtkSignalFunc) SquareWindow,
                            NULL);

   gtk_toolbar_append_space (GTK_TOOLBAR (toolbar));

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Zoom In", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_zoomin),
                            (GtkSignalFunc) ZoomIn,
                            NULL);

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Zoom Out", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_zoomout),
                            (GtkSignalFunc) ZoomOut,
                            NULL);

   gtk_toolbar_append_item (GTK_TOOLBAR (toolbar),
                            NULL, "Default View", "",
                            CreateWidgetFromXpm (vbox_main, (gchar **) xpm_default),
                            (GtkSignalFunc) DefaultView,
                            NULL);
}

//*********************************************************************
// called by repaint
//
// uses globals: anaglyphic, model
// sets global:  ana_line
void make_anaglyphic( three_d_point* p1, three_d_point* p2 ) {
   gdouble red_x1, red_y1, cyan_x1, cyan_y1;
   gdouble red_x2, red_y2, cyan_x2, cyan_y2;

   // translate 3-d to two 2-d's via anaglyphic
   g3dav_ana_translate( anaglyphic, p1->x, p1->y, p1->z );

   red_x1 = anaglyphic->red_x;
   red_y1 = anaglyphic->red_y;
   // translate world space to screen space
   ana_line->rx1 = (gint)(((red_x1-model->w_xmin)/(model->w_xmax-model->w_xmin))*Swidth);
   ana_line->ry1 = (gint)(Sheight-( ((red_y1-model->w_ymin)/(model->w_ymax-model->w_ymin))*Sheight));

   cyan_x1 = anaglyphic->cyan_x;
   cyan_y1 = anaglyphic->cyan_y;
   // translate world space to screen space
   ana_line->cx1 = (gint)(((cyan_x1-model->w_xmin)/(model->w_xmax-model->w_xmin))*Swidth);
   ana_line->cy1 = (gint)(Sheight-( ((cyan_y1-model->w_ymin)/(model->w_ymax-model->w_ymin))*Sheight));

   g3dav_ana_translate( anaglyphic, p2->x, p2->y, p2->z );
   red_x2 = anaglyphic->red_x;
   red_y2 = anaglyphic->red_y;
   // translate world space to screen space
   ana_line->rx2 = (gint)(((red_x2-model->w_xmin)/(model->w_xmax-model->w_xmin))*Swidth);
   ana_line->ry2 = (gint)(Sheight-( ((red_y2-model->w_ymin)/(model->w_ymax-model->w_ymin))*Sheight));

   cyan_x2 = anaglyphic->cyan_x;
   cyan_y2 = anaglyphic->cyan_y;
   // translate world space to screen space
   ana_line->cx2 = (gint)(((cyan_x2-model->w_xmin)/(model->w_xmax-model->w_xmin))*Swidth);
   ana_line->cy2 = (gint)(Sheight-( ((cyan_y2-model->w_ymin)/(model->w_ymax-model->w_ymin))*Sheight));
}

//*********************************************************************
// big ugly function that paints the model on the canvas3d
//
// this will be cleaned up ASAP
gint repaint( gpointer data ) {

   GtkWidget* drawing_area = (GtkWidget*)data;
   GdkDrawable *drawable;
   GdkRectangle update_rect;
   //guint x1, y1, x2, y2, 
   guint width, height;
   GSList *node;
   GSList *vertices;
   three_d_point *point;
   line_segment *line_seg;
   three_d_point *point_array;
   label_3d* label3d;

   gdouble red_x1, red_y1, cyan_x1, cyan_y1;
   gdouble red_x2, red_y2, cyan_x2, cyan_y2;
   gint rx1, ry1, cx1, cy1;
   gint rx2, ry2, cx2, cy2;
   gint i;

   three_d_point o_axis1, o_axis2;

   //g_print( "IN REPAINT RED =    : (r %x) (g %x) (b %x)\n\n", ColorRed->red,ColorRed->green,ColorRed->blue );
   //g_print( "drawing_area @ %ld\n", drawing_area );
   //g_print( "      canvas3d @ %ld\n", canvas3d );

   g_assert( drawing_area!=NULL);

   drawing_area->style->font = gdk_font_load("fixed");

   width = drawing_area->allocation.width;
   height = drawing_area->allocation.height;

   drawable = drawing_area->window;
   g_assert(drawable!=NULL);

   if (!offscreenBuffer) {
      offscreenBuffer = gdk_pixmap_new( drawable, width, height, -1 );
   }

   // clear the offscreen buffer
   gdk_draw_rectangle( offscreenBuffer, drawing_area->style->black_gc, 
                       TRUE, 0, 0,
                       drawing_area->allocation.width,
                       drawing_area->allocation.height );

   if (model) {
      //yrot( matrix, 5 );
      // get a list of transformed vertices
      vertices = transform(matrix, model->vertices);
      // copy to an array to make indexed lookups faster
      point_array = g_malloc(model->nvert*sizeof(three_d_point));
      g_assert(point_array!=NULL);
      for (i=0, node = vertices; node && i<model->nvert; node = node->next) {
         point = (three_d_point*)node->data;
         //g_print( "copy point %d: (%f, %f, %f)\n", i, point->x, point->y, point->z );
         memcpy(point_array+i, node->data, sizeof(three_d_point));
         i++;
      }

      // ---------------------------------------------------
      // NEED TO DO: Consider z-depth
      // sort the point_array by z-value
      // original DOS code had this
      // create an array with values = index into point_array
      // 
      // no, no wait. need to sort the line list by z-depth!
      // ---------------------------------------------------

      // ========================================
      // draw the lines in the buffer
      for (node = model->lines; node; node = node->next) {
         line_seg = node->data;
         g_assert(line_seg!=NULL);

         make_anaglyphic( point_array+(line_seg->point1-1), point_array+(line_seg->point2-1) );

         if (penRed!=NULL) {
            // draw red line
            gdk_draw_line( offscreenBuffer, (GdkGC*)penRed, 
                           ana_line->rx1, ana_line->ry1, ana_line->rx2, ana_line->ry2 );
         }
         if (penCyan!=NULL) {
            // *** draw the cyan line ***
            gdk_draw_line( offscreenBuffer, (GdkGC*)penCyan, 
                           ana_line->cx1, ana_line->cy1, ana_line->cx2, ana_line->cy2 );
         }

      } // end draw lines

      // ========================================
      // draw the labels
      for (node = model->labels; node; node = node->next) {
         label3d = node->data;
         g_assert(label3d!=NULL);

         point = point_array+(label3d->point-1);

         g3dav_ana_translate( anaglyphic, point->x, point->y, point->z );

         red_x1 = anaglyphic->red_x;
         red_y1 = anaglyphic->red_y;
         // translate world space to screen space
         rx1 = (gint)(((red_x1-model->w_xmin)/(model->w_xmax-model->w_xmin))*width);
         ry1 = (gint)(height-( ((red_y1-model->w_ymin)/(model->w_ymax-model->w_ymin))*height));

         cyan_x1 = anaglyphic->cyan_x;
         cyan_y1 = anaglyphic->cyan_y;
         // translate world space to screen space
         cx1 = (gint)(((cyan_x1-model->w_xmin)/(model->w_xmax-model->w_xmin))*width);
         cy1 = (gint)(height-( ((cyan_y1-model->w_ymin)/(model->w_ymax-model->w_ymin))*height));

         // draw red string
         gdk_draw_string( offscreenBuffer, drawing_area->style->font, 
                          (GdkGC*)penRed, rx1, ry1, (gchar*)label3d->label );
         // draw cyan string
         gdk_draw_string( offscreenBuffer, drawing_area->style->font, 
                          (GdkGC*)penCyan, cx1, cy1, (gchar*)label3d->label );
      } // end labels

      // ========================================
      // draw rotation axis
      if (model->show_rotation_axes) {

         gdk_draw_line( offscreenBuffer, drawing_area->style->white_gc,
                        drawing_area->allocation.width/2, 0,
                        drawing_area->allocation.width/2, Sheight );

         gdk_draw_line( offscreenBuffer, drawing_area->style->white_gc,
                        0,drawing_area->allocation.height/2,
                        Swidth,drawing_area->allocation.height/2 );

      } // end draw axis

      // ========================================
      // draw origin axis
      if (model->show_origin_axes) {

         vertices = transform(matrix, model->origin_axes); // should be six of these
         for (i=0, node = vertices; node && i<model->nvert; node = node->next) {
            point = (three_d_point*)node->data;
            //g_print( "copy point %d: (%f, %f, %f)\n", i, point->x, point->y, point->z );
            memcpy(point_array+i, node->data, sizeof(three_d_point));
            i++;
         }

         node = vertices;  // this SHOULD = x1, i need to verify if correct with g_slist 

         make_anaglyphic( (three_d_point*)node->data, (three_d_point*)node->next->data );
         if (penRed!=NULL) {
            gdk_draw_line( offscreenBuffer, (GdkGC*)penRed, 
                           ana_line->rx1, ana_line->ry1, ana_line->rx2, ana_line->ry2 );
         }
         if (penCyan!=NULL) {
            gdk_draw_line( offscreenBuffer, (GdkGC*)penCyan, 
                           ana_line->cx1, ana_line->cy1, ana_line->cx2, ana_line->cy2 );
         }
         gdk_draw_string( offscreenBuffer, drawing_area->style->font, 
                          (GdkGC*)penRed, ana_line->rx2, ana_line->ry2, "X" );
         gdk_draw_string( offscreenBuffer, drawing_area->style->font, 
                          (GdkGC*)penCyan, ana_line->cx2, ana_line->cy2, "X" );

         node=node->next->next;

         make_anaglyphic( (three_d_point*)node->data, (three_d_point*)node->next->data );
         if (penRed!=NULL) {
            gdk_draw_line( offscreenBuffer, (GdkGC*)penRed, 
                           ana_line->rx1, ana_line->ry1, ana_line->rx2, ana_line->ry2 );
         }
         if (penCyan!=NULL) {
            gdk_draw_line( offscreenBuffer, (GdkGC*)penCyan, 
                           ana_line->cx1, ana_line->cy1, ana_line->cx2, ana_line->cy2 );
         }
         gdk_draw_string( offscreenBuffer, drawing_area->style->font, 
                          (GdkGC*)penRed, ana_line->rx2, ana_line->ry2, "Y" );
         gdk_draw_string( offscreenBuffer, drawing_area->style->font, 
                          (GdkGC*)penCyan, ana_line->cx2, ana_line->cy2, "Y" );
         node=node->next->next;

         make_anaglyphic( (three_d_point*)node->data, (three_d_point*)node->next->data );
         if (penRed!=NULL) {
            gdk_draw_line( offscreenBuffer, (GdkGC*)penRed, 
                           ana_line->rx1, ana_line->ry1, ana_line->rx2, ana_line->ry2 );
         }
         if (penCyan!=NULL) {
            gdk_draw_line( offscreenBuffer, (GdkGC*)penCyan, 
                           ana_line->cx1, ana_line->cy1, ana_line->cx2, ana_line->cy2 );
         }
         gdk_draw_string( offscreenBuffer, drawing_area->style->font, 
                          (GdkGC*)penRed, ana_line->rx2, ana_line->ry2, "Z" );
         gdk_draw_string( offscreenBuffer, drawing_area->style->font, 
                          (GdkGC*)penCyan, ana_line->cx2, ana_line->cy2, "Z" );

      } // end draw axis

   } // end if(model)

   // Now draw the canvas3d to the screen

   update_rect.x=0;
   update_rect.y=0;
   update_rect.width=drawing_area->allocation.width;
   update_rect.height=drawing_area->allocation.height;
   // this causes an expose_event signal
   gtk_widget_draw( drawing_area, &update_rect );

   // has to return TRUE otherwise the timeout will stop calling it
   return(TRUE);
   //return (FALSE);
} //end repaint

//*********************************************************************
// this event handler should be called whenever the drawable is created
// or resized
static gint configure_event( GtkWidget *widget, GdkEventConfigure *event) {
   // this method assumes the widget is the canvas3d

   //g_print( "SIGNAL: configure_event\n" );


   // set screen width & height
   Swidth = widget->allocation.width;
   Sheight = widget->allocation.height;

   // free buffer if already created
   if (offscreenBuffer) {
      gdk_pixmap_unref( offscreenBuffer );
   }
   // create a new buffer with new size
   offscreenBuffer = gdk_pixmap_new( widget->window,
                                     widget->allocation.width,
                                     widget->allocation.height,
                                     -1 );

   if (penRed==NULL) {
      penRed = gdk_gc_new(offscreenBuffer);
      if (ColorRed==NULL) {
         ColorRed = g_malloc(sizeof(GdkColor));
         ColorRed->red   =0xffff;
         ColorRed->green =0x0000;
         ColorRed->blue  =0x0000;
         gdk_color_alloc( gdk_colormap_get_system(), ColorRed );
      }
      gdk_gc_set_foreground( penRed, ColorRed );
   }

   if (penCyan==NULL) {
      penCyan = gdk_gc_new(offscreenBuffer);
      if (ColorCyan==NULL) {
         ColorCyan = g_malloc(sizeof(GdkColor));
         ColorCyan->red   =0x0000;
         ColorCyan->green =0xffff;
         ColorCyan->blue  =0xffff;
         gdk_color_alloc( gdk_colormap_get_system(), ColorCyan );
      }
      gdk_gc_set_foreground( penCyan, ColorCyan );
   }

   /*
   if (model) {
      // testing bounding box
      // make it square
      model->w_xmin = min_of_3(model->xmin,model->ymin,model->zmin)*1.5;
      model->w_xmax = max_of_3(model->xmax,model->ymax,model->zmax)*1.5;

      model->w_xmax = fabs(model->w_xmin) > fabs(model->w_xmax) ? fabs(model->w_xmin) : fabs(model->w_xmax);
      model->w_xmin = -(model->w_xmax);
      //g_print( "MIN %f\n", model->w_xmin );
      //g_print( "MAX %f\n", model->w_xmax );
      model->w_ymin = model->w_xmin;
      model->w_ymax = model->w_xmax;
      //Wzmin = model->zmin;
      //Wzmax = model->zmax;
   }
   */

   repaint(widget);

   return(TRUE);
} // end configure_event

//*********************************************************************
// this event handler should be called whenever the window is exposed
// to the viewer or the gdk_widget_draw routine is called.
gint expose_event( GtkWidget *widget, GdkEventExpose *event) {
   // copy the offscreen pixmap to the window
   if (widget!=NULL && offscreenBuffer!=NULL) {
      gdk_draw_pixmap( widget->window,
                       widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
                       offscreenBuffer,
                       event->area.x, event->area.y,
                       event->area.x, event->area.y,
                       event->area.width, event->area.height );
   }

   return(FALSE);
} //end expose_event

gint mouse1Drag( GtkWidget *widget, gint x, gint y ) {

   double xtheta = (prev_mouse1y - y) * (360.0f / Swidth );
   double ytheta = (prev_mouse1x - x) * (360.0f / Sheight );

   if (!matrix) {
      // there's no model in the viewer at the moment
      return(TRUE);
   }
   //g_print( "drag (%d, %d) xtheta=%f   ytheta=%f\n", x, y, xtheta, ytheta );

   yrot( matrix, ytheta );
   xrot( matrix, xtheta );
   repaint(widget);

   return(TRUE);
} // end mouse1Drag

//*********************************************************************
// adjust world size (zoom in and out)
// this seems to be pretty damn buggy
gint mouse3Drag( GtkWidget *widget, gint x, gint y ) {

   /*
    gint ydelta = prev_mouse3y - y;
    double percent_delta = fabs((double)ydelta/Sheight);
    double world_delta = (prev_wxmax - prev_wxmin) * percent_delta;
 
    if (ydelta>0) {
       world_delta = - (world_delta);
    }
    model->w_xmin = model->w_ymin = (prev_wxmin - (world_delta / 2) );
    model->w_xmax = model->w_ymax = (prev_wxmax + (world_delta / 2) );
 
   */
   //g_print( "drag 2 y=%d ydelta=%d Sheight=%d percent=%f model->w_xmin=%f model->w_xmax=%f\n", y, ydelta, Sheight,percent_delta, model->w_xmin, model->w_xmax );

   repaint(widget);

   return(TRUE);
} // end mouse3Drag


//*********************************************************************
gint motion_notify_event( GtkWidget *widget, GdkEventMotion *event ) {
   gint x, y;
   GdkModifierType state;

   if (event->is_hint) {
      gdk_window_get_pointer( event->window, &x, &y, &state );
   } else {
      x=event->x;
      y=event->y;
      state=event->state;
   }

   //g_print( "mouse motion (%d, %d)\n", x, y );

   // button is pressed
   if ((state & GDK_BUTTON1_MASK)) {
      //if ((state & GDK_BUTTON1_MASK) && (offscreenBuffer != NULL)) {
      mouse1Drag( widget, x, y );
   }
   prev_mouse1x = x;
   prev_mouse1y = y;

   if ((state & GDK_BUTTON3_MASK)) {
      //if ((state & GDK_BUTTON1_MASK) && (offscreenBuffer != NULL)) {
      mouse3Drag( widget, x, y );
   }

   return(TRUE);
} // end motion_notify_event

//*********************************************************************
gint button_press_event( GtkWidget *widget, GdkEventMotion *event ) {
   gint x, y;
   GdkModifierType state;

   if (event->is_hint) {
      gdk_window_get_pointer( event->window, &x, &y, &state );
   } else {
      x=event->x;
      y=event->y;
      state=event->state;
   }

   if ((state & GDK_BUTTON3_MASK)) {
      prev_mouse3x = x;
      prev_mouse3y = y;
      prev_wxmin = model->w_xmin;
      prev_wxmax = model->w_xmax;
      //g_print( "SET prev_wxmin=%f prev_wxmax=%f\n", prev_wxmin, prev_wxmax );
   }

   return(TRUE);
} // end button_press_event

//*********************************************************************
static void ColorAdjust( gpointer callback_data, guint callback_action, GtkWidget *widget) {

   g3dav_color_dialog( penRed, penCyan, ColorRed, ColorCyan );


} // end ColorAdjust

//*********************************************************************
static void ParamAdjust( gpointer callback_data, guint callback_action, GtkWidget *widget) {

   if (parameters==NULL) {
      parameters = g_malloc( sizeof( g3dav_parameters ) );
   }
   parameters->world_xmin = 1.234567;
   parameters->eye_dis = &(anaglyphic->eye_dis);
   parameters->view_dis = &(anaglyphic->view_dis);
   parameter_adjust_dialog( parameters );

} // end ParamAdjust


//*********************************************************************
static void DefaultView( gpointer callback_data, guint callback_action, GtkWidget *widget) {
   setBoundingBox( model );
   repaint(canvas3d);
} // end SquareWindow

//*********************************************************************
static void ZoomOut( gpointer callback_data, guint callback_action, GtkWidget *widget) {

   gdouble range_x, range_y;
   //gdouble mid_x, mid_y;

   range_x = model->w_xmax - model->w_xmin;
   range_y = model->w_ymax - model->w_ymin;

   range_x *= 0.5;
   range_y *= 0.5;

   model->w_xmin -= range_x;
   model->w_xmax += range_x;

   model->w_ymin -= range_y;
   model->w_ymax += range_y;

   repaint(canvas3d);
} // end SquareWindow

//*********************************************************************
static void ZoomIn( gpointer callback_data, guint callback_action, GtkWidget *widget) {

   gdouble range_x, range_y;
   //gdouble mid_x, mid_y;

   range_x = model->w_xmax - model->w_xmin;
   range_y = model->w_ymax - model->w_ymin;

   range_x *= 0.25;
   range_y *= 0.25;

   model->w_xmin += range_x;
   model->w_xmax -= range_x;

   model->w_ymin += range_y;
   model->w_ymax -= range_y;

   repaint(canvas3d);
} // end SquareWindow


static void SquareWindow( gpointer callback_data, guint callback_action, GtkWidget *widget) {
   //GtkRequisition req;

   //g_print("square not working\n");

   NotReady( callback_data, callback_action, widget );

   //repaint(canvas3d);

} // end SquareWindow

//*********************************************************************
static void ToggleRAxis( gpointer callback_data, guint callback_action, GtkWidget *widget) {
   model->show_rotation_axes = model->show_rotation_axes ? (FALSE) : (TRUE);
   repaint(canvas3d);
} // end SquareWindow

//*********************************************************************
static void ToggleOAxis( gpointer callback_data, guint callback_action, GtkWidget *widget) {
   model->show_origin_axes = model->show_origin_axes ? (FALSE) : (TRUE);
   repaint(canvas3d);
} // end SquareWindow

//*********************************************************************
// use a file dialog to allow user to pick a file, then try to load it
static void NotReady( gpointer callback_data, guint callback_action, GtkWidget *widget) {

   message_dialog( "This feature not implemented yet, sorry." );

}

//*********************************************************************
// use a file dialog to allow user to pick a file, then try to load it
static void ShowNext( gpointer callback_data, guint callback_action, GtkWidget *widget) {


   //g_print( "show next\n" );
   if (!CurrentModel || CurrentModel->prev==NULL) {
      //g_print( "  no next\n" );
      return;
   }

   CurrentModel = CurrentModel->prev;
   g_assert(CurrentModel!=NULL);
   model = (g3dav_model*)CurrentModel->data;

   CurrentMatrix = CurrentMatrix->prev;
   g_assert(CurrentMatrix!=NULL);
   matrix= (g3dav_matrix*)CurrentMatrix->data;
   repaint(canvas3d);
} // end ShowNext

//*********************************************************************
// note: GList->next is the previous thing added
static void ShowPrev( gpointer callback_data, guint callback_action, GtkWidget *widget) {

   //g_print( "show prev\n" );
   if (!CurrentModel || CurrentModel->next==NULL) {
      //g_print( "  no prev\n" );
      return;
   }

   CurrentModel = CurrentModel->next;
   g_assert(CurrentModel!=NULL);
   model = (g3dav_model*)CurrentModel->data;

   CurrentMatrix = CurrentMatrix->next;
   g_assert(CurrentMatrix!=NULL);
   matrix= (g3dav_matrix*)CurrentMatrix->data;
   repaint(canvas3d);
} // end ShowPrev

/* Get the selected filename and print it to the console */
void file_ok_sel (GtkWidget *w, GtkFileSelection *fs) {
   gchar* filename;

   g3dav_model *new_model;
   g3dav_matrix *new_matrix;

   new_model = g3dav_model_new();

   filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION (fs));

   if ( load_model( new_model, filename ) ) {

      // get rid of the file selection dialog
      gtk_widget_destroy( GTK_WIDGET(fs) );

      models = g_list_prepend( models, new_model );
      CurrentModel = models;
      model = new_model;

      new_matrix = g3dav_matrix_new();
      matrices = g_list_prepend( matrices, new_matrix );
      CurrentMatrix=matrices;
      matrix = new_matrix;

      // translates matrix to 0,0,0 and then alters models world space
      // to keep model in centered view
      CenterModel(model, matrix);

      //g3dav_model_destroy(model);
      repaint(canvas3d);

   } else {

      g3dav_model_destroy(new_model);
      g_message( "Could not load %s\n", filename);

   }
} // end file_ok_sel



// **********************************************************
// translate the matrix and the wolrdspace so that the model 
// will be drawn centered in worldspace and rotating about
// the center of the model
void CenterModel(g3dav_model* _model, g3dav_matrix* _matrix) {

   gdouble xmidpoint, ymidpoint, zmidpoint;

   xmidpoint = (_model->xmax + _model->xmin) / 2;
   ymidpoint = (_model->ymax + _model->ymin) / 2;
   zmidpoint = (_model->zmax + _model->zmin) / 2;

   translate( _matrix, -(xmidpoint), -(ymidpoint), -(zmidpoint) );
   model->w_xmin-=xmidpoint;
   model->w_xmax-=xmidpoint;
   model->w_ymin-=ymidpoint;
   model->w_ymax-=ymidpoint;

}



//*********************************************************************
// use a file dialog to allow user to pick a file, then try to load it
static void LoadModel( gpointer callback_data, guint callback_action, GtkWidget *widget) {
   GtkWidget* filedlg;

   filedlg = gtk_file_selection_new( "Open" );
   /* Connect the ok_button to file_ok_sel function */
   gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION (filedlg)->ok_button),
                       "clicked", (GtkSignalFunc) file_ok_sel, filedlg );

   /* Connect the cancel_button to destroy the widget */
   gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION(filedlg)->cancel_button),
                              "clicked", (GtkSignalFunc) gtk_widget_destroy,
                              GTK_OBJECT (filedlg));

   gtk_widget_show(filedlg);


} // end LoadModel

//*********************************************************************
// use a file dialog to allow user to pick a file, then try to load it
static void CloseModel( gpointer callback_data, guint callback_action, GtkWidget *widget) {

   GList *closingModel;
   GList *closingMatrix;
   g3dav_model *byebye_model;
   g3dav_matrix *byebye_matrix;

   // NEED TO DO check updated flag in model, then maybe query save as 
   //g_print( "close \n" );

   if (CurrentModel==NULL) {
      //g_print( "  nothing to close\n" );
      return;
   }
   closingModel = CurrentModel;
   closingMatrix = CurrentMatrix;

   // first update display
   if (CurrentModel->prev!=NULL) {

      ShowNext( NULL, 0, NULL );  // 

   } else if (CurrentModel->next!=NULL) {

      ShowPrev( NULL, 0, NULL );

   } else {

      // last one, clean the slate
      CurrentModel = NULL;
      model = NULL;
      models = NULL;
      CurrentMatrix = NULL;
      matrix= NULL;
      matrices = NULL;
      repaint(canvas3d);

   }

   // now remove this model & matrix from our window list
   byebye_model = (g3dav_model *)closingModel->data;
   byebye_matrix = (g3dav_matrix *)closingMatrix->data;
   models = g_list_remove( models, byebye_model );
   matrices = g_list_remove( matrices, byebye_matrix );

   // and free this space
   g3dav_model_destroy( byebye_model );
   g3dav_matrix_destroy( byebye_matrix );

} // end CloseModel

//*********************************************************************
// Straight from the Eric Harlow GTK+ Programming book
GtkWidget *CreateWidgetFromXpm( GtkWidget *win, gchar **xpm_data) {
   GdkBitmap *mask;
   GdkPixmap *pixmap_data;
   GtkWidget *pixmap_widget;

   pixmap_data = gdk_pixmap_create_from_xpm_d( win->window, &mask, NULL, xpm_data);

   pixmap_widget = gtk_pixmap_new( pixmap_data, mask );

   gtk_widget_show( pixmap_widget );

   return(pixmap_widget);
}  // end CreateWidgetFromXpm

//*********************************************************************
//*********************************************************************
int main (int argc, char** argv) {
   GtkWidget *box1;
   //GtkWidget *toolbar;
   GtkAccelGroup *accel_group;
   GtkItemFactory *item_factory;
   //GtkWidget *window;

   // only need one of these, get the space now.
   ana_line = g_malloc(sizeof(anaglyphic_line));          // used by repaint
   if (!ana_line) {
      g_error( "Cannot get necessary memory space from operating system!\n" );
   }

   // set up model
   model = g3dav_model_new();
   models = g_list_prepend( models, model );
   CurrentModel=models;

   // load an initial model
   addVertex( model, 0.0, 0.0, 1 );
   addVertex( model, 0, -1, 0 ); //2
   addVertex( model, -1, 0, 0 ); //3
   addVertex( model, 0, 1, 0 );  //4
   addVertex( model, 1, 0, 0 );  //5
   addVertex( model, 0, 0, -1 ); //6
   addLine( model, 2, 6);
   addLine( model, 3, 6);
   addLine( model, 4, 6);
   addLine( model, 5, 6);
   addLabel( model, 1, "g3dav" );
   addLabel( model, 2, "GNU" );
   addLabel( model, 3, "3D" );
   addLabel( model, 4, "Anaglyphic" );
   addLabel( model, 5, "Viewer" );
   //addLine( model, 2, 6);
   //dumpModel(model);

   // set up anaglyphic converter
   anaglyphic = g3dav_ana_new();

   // set up matrix
   matrix = g3dav_matrix_new();
   xrot(matrix, 70);
   yrot(matrix, 2800);
   matrices = g_list_prepend( matrices, matrix );
   CurrentMatrix=matrices;

   setBoundingBox( model );

   // let's get busy
   gtk_init( &argc, &argv );

   window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
   gtk_window_set_title( GTK_WINDOW(window), "Gnu 3D Anaglyphic Viewer" );
   gtk_signal_connect( GTK_OBJECT(window), "delete_event",
                       GTK_SIGNAL_FUNC(AnyLastRequests), NULL );

   canvas3d = gtk_drawing_area_new();
   //g_print( "      canvas3d @ %ld\n", canvas3d );

   // Listen only for these events (best to set this ASAP, before widget is realized)
   gtk_widget_set_events( canvas3d, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
                          GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK );

   gtk_drawing_area_size( GTK_DRAWING_AREA(canvas3d), 300, 300 );

   accel_group = gtk_accel_group_new();
   item_factory = gtk_item_factory_new( GTK_TYPE_MENU_BAR, "<yada>", accel_group );
   gtk_item_factory_create_items( item_factory, 
                                  sizeof(menu_items) / sizeof(menu_items[0]),
                                  menu_items, NULL );
   gtk_accel_group_attach( accel_group, GTK_OBJECT(window) );

   // start packing boxes
   box1 = gtk_vbox_new( FALSE, 0 );
   gtk_container_add( GTK_CONTAINER(window), box1 );

   gtk_box_pack_start( GTK_BOX(box1), 
                       gtk_item_factory_get_widget( item_factory, "<yada>" ),
                       FALSE, FALSE, 0 );

   gtk_widget_show(box1);

   CreateToolbar( GTK_WIDGET(box1) );

   gtk_box_pack_start( GTK_BOX(box1), canvas3d, TRUE, TRUE, 0 );

   gtk_widget_show_all(window);


   gtk_signal_connect( GTK_OBJECT(canvas3d), "expose_event", 
                       GTK_SIGNAL_FUNC(expose_event), NULL );

   gtk_signal_connect( GTK_OBJECT(canvas3d), "configure_event", 
                       GTK_SIGNAL_FUNC(configure_event), NULL );

   gtk_signal_connect( GTK_OBJECT(canvas3d), "motion_notify_event", 
                       GTK_SIGNAL_FUNC(motion_notify_event), NULL );

   gtk_signal_connect( GTK_OBJECT(canvas3d), "button_press_event", 
                       GTK_SIGNAL_FUNC(button_press_event), NULL );

   //gtk_timeout_add( 1000, repaint, (gpointer)canvas3d );

   configure_event( canvas3d, NULL ); // sticking this here because it was not
   // being called on startup on solaris

   gtk_main();

   exit(0);
} // end main
