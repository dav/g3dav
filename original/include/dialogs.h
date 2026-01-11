#ifndef G3DAV_DIALOGS
#define G3DAV_DIALOGS

#include <gtk/gtk.h>

void message_dialog( gchar* message );
void close_dialog( GtkWidget *widget, gpointer data );
void cancel_dialog( GtkWidget *widget, gpointer data );
gint destroy_dialog( GtkWidget *widget, gpointer data ) ;

void g3dav_color_dialog( GdkGC* penR, GdkGC* penC, 
                         GdkColor *ColorR, GdkColor *ColorC );

gint g3dav_color_dlg_configure_event( GtkWidget *widget, GdkEventConfigure *event);
gint g3dav_color_dlg_expose_event( GtkWidget *widget, GdkEventExpose *event);
gint repaint_color_dlg_canvas( gpointer data );

typedef struct {
  GtkWidget *dialog;
  GdkColor *color;
  GdkColor *original_color;
} typColorDialogInfo;


typedef struct {
  GtkSignalFunc cc_callback;
  GtkSignalFunc cancel_callback;
  GtkSignalFunc ok_callback;
} ColorDialogCallbacks;

typedef struct {
  gdouble world_xmin;
  gdouble world_xmax;
  gdouble world_ymin;
  gdouble world_ymax;
  gdouble *eye_dis;
  gdouble *view_dis;
} g3dav_parameters;

typedef struct {
  g3dav_parameters *p;
  GtkEntry* eye;
  GtkEntry* view;
  GtkWidget* dialog;
} param_update_pkg;

void do_color_select( GtkWidget* w, gpointer* data );
void parameter_adjust_dialog( g3dav_parameters* p );
gint destroy_param_dialog( GtkWidget *widget, gpointer data );
void update_parameters( GtkWidget *widget, gpointer data );


#endif
