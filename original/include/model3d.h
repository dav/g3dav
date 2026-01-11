#ifndef _G3DAV_MODEL3D
#define _G3DAV_MODEL3D

#include <gtk/gtk.h>
#include <stdlib.h>

typedef struct three_d_point {
   gfloat x;
   gfloat y;
   gfloat z;
} three_d_point;

typedef struct line_segment {
   guint point1;
   guint point2;
} line_segment;

typedef struct label_3d {
   guint point;
   gchar *label;
} label_3d;


typedef struct g3dav_model {
   gchar *name;        // unused at the moment

   GSList *vertices;     // linked list of vertices
   guint  nvert;         // number of vertices
   GSList *lines;        // linked list of lines
   guint  nline;         // number of lines
   GSList *labels;       // linked list of labels (with 3d point)
   guint  nlabel;        // number of labels

   GSList *origin_axes;  // linked list of 6 points that draw the origin axes

   gfloat  xmin, xmax, ymin, ymax, zmin, zmax; // the bounding cube

   gdouble w_xmin, w_xmax, w_ymin, w_ymax;     // world bounds, these maybe should not
                                               // not be with the model "object"

   gboolean show_rotation_axes;
   gboolean show_origin_axes;

   //three_d_point x_axis1, x_axis2, y_axis1, y_axis2, z_axis1, z_axis2;

} g3dav_model;



// returns a new and initialized g3dav_model
g3dav_model *g3dav_model_new();
void g3dav_model_destroy(g3dav_model *model);

gint  addVertex( g3dav_model* model, gfloat x, gfloat y, gfloat z );
gint  addLine( g3dav_model* model, guint point1, guint point2 );
gint  addLabel( g3dav_model* model, guint point, gchar* label );
//void  transform(g3dav_model* model);
void  setBoundingBox(g3dav_model* model);

void  dumpModel(g3dav_model* model);
void  dump_vertex(gpointer _vertex, gpointer arg);
void  dump_line(gpointer _line, gpointer arg);
gint load_model( g3dav_model* model, char* filename );
gint load_OBJ_Model( g3dav_model* model, char* filename );
gint load_PDB_Model( g3dav_model* model, char* filename );
gint load_CSV_Model( g3dav_model* model, char* filename ); 


#endif //_G3DAV_MODEL3D
