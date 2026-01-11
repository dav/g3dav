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

#include <ctype.h>
#include <stdio.h>
#include <strings.h>

#include <math.h>

#include "model3d.h"

// ********************************************************************
g3dav_model *g3dav_model_new() {

   g3dav_model *model;

   model =  g_malloc(sizeof(g3dav_model));
   if (model) {
      model->vertices=NULL;    
      model->nvert=0;

      model->lines=NULL;       
      model->nline=0;

      model->labels=NULL;       
      model->nlabel=0;

      model->origin_axes=NULL;

      model->xmin=0;
      model->xmax=0;
      model->ymin=0;
      model->ymax=0;
      model->zmin=0;
      model->zmax=0;

      model->show_rotation_axes=FALSE;
      model->show_origin_axes=FALSE;
   }

   return model;
}

// ********************************************************************
// say no to memory leaks
void g3dav_model_destroy(g3dav_model *model) {
   GSList *node;
   label_3d *label3d;

   //g_print( "in model_destroy()\n" );
   // destroy all the points
   for (node = model->vertices; node; node = node->next) {
      g_free(node->data);
   }
   // destroy all the line_segs
   for (node = model->lines; node; node = node->next) {
      g_free(node->data);
   }
   // destroy all the labels
   for (node = model->labels; node; node = node->next) {
      label3d = node->data;
      g_free((gchar*)(label3d->label)); // free label string
      g_free(node->data);               // free label
   }
   // destroy the model
   g_free(model);

}


// ********************************************************************
gint  addVertex( g3dav_model* model, gfloat x, gfloat y, gfloat z ) {

   three_d_point *point;

   point = g_malloc(sizeof(three_d_point));
   if (point) {
      point->x = x;
      point->y = y;
      point->z = z;

      //g_print( "adding (%0.2f,%0.2f,%0.2f)\n", point->x, point->y, point->z );

      model->vertices = g_slist_append( model->vertices, point );
      model->nvert++;
   } else {
      g_warning( "unable to allocate memory for new point (%0.2f,%0.2f,%0.2f)\n", x, y, z );
   }
   return model->nvert;
}

// ********************************************************************
void  dump_vertex(gpointer _vertex, gpointer arg) {
   three_d_point* vertex;

   vertex = (three_d_point*)_vertex;

   g_print( "point: (%0.5f, %0.5f, %0.5f)\n", vertex->x, vertex->y, vertex->z );
}


// ********************************************************************
gint  addLine( g3dav_model* model, guint point1, guint point2 ) {
   line_segment *line;

   line = g_malloc(sizeof(line_segment));
   if (line) {

      if (point1 > point2) {
         guint t = point1;
         point1 = point2;
         point2 = t;
      }

      line->point1 = point1;
      line->point2 = point2;

      //g_print( "adding line (%d,%d)\n", line->point1, line->point2 );

      model->lines = g_slist_append( model->lines, line );
      model->nline++;
      //con[i] = (p1 << 16) | p2;

   } else {

      g_warning( "unable to allocate memory for new line (%d,%d)\n", line->point1, line->point2 );

   }
   return model->nline;

}

// ********************************************************************
void  dump_line(gpointer _line, gpointer arg) {
   line_segment* line;

   line = (line_segment*)_line;

   g_print( "line from point %d to %d.\n", line->point1, line->point2 );
}

// ********************************************************************
gint  addLabel( g3dav_model* model, guint point, gchar* label ) {

   label_3d *label3d;

   label3d = g_malloc(sizeof(label_3d));
   if (label3d) {
      label3d->point = point;
      label3d->label = g_strdup(label);
      //g_print( "adding (%d) %s\n", label3d->point, label3d->label );

      model->labels = g_slist_append( model->labels, label3d );
      model->nlabel++;
   } else {
      g_warning( "unable to allocate memory for new 3d label (%d) %s\n", point, label );
   }
   return model->nlabel;
}

// ********************************************************************
void  dump_label(gpointer _label, gpointer arg) {
   label_3d* label;

   label = (label_3d*)_label;
   g_print( "label: (%d) %s\n", label->point, label->label );
}


// ********************************************************************
void  dumpModel(g3dav_model* model) {
   g_print( "Model dump (%d vertices) (%d lines) (%d labels):\n", 
            g_slist_length(model->vertices),
            g_slist_length(model->lines),
            g_slist_length(model->labels) );

   g_slist_foreach( model->vertices, (GFunc)dump_vertex, NULL );
   g_slist_foreach( model->lines, (GFunc)dump_line, NULL );
   g_slist_foreach( model->labels, (GFunc)dump_label, NULL );
}




//*********************************************************************
gdouble min_of_3( gdouble hubristic, gdouble impatient, gdouble lazy ) {
   gdouble virtue;
   virtue = hubristic < impatient ? hubristic : impatient;
   return lazy < virtue ? lazy : virtue;
}

//*********************************************************************
gdouble max_of_3( gdouble surf, gdouble skate, gdouble snowboard ) {
   gdouble la_glise;
   la_glise = surf > skate ? surf : skate;
   return snowboard > la_glise ? snowboard : la_glise;
}

//**********************************************************
void add_origin_axes_point( g3dav_model* model, gdouble x, gdouble y, gdouble z ) {
   three_d_point *point;

   point = g_malloc(sizeof(three_d_point));
   if (point) {
      point->x = x;
      point->y = y;
      point->z = z;
      model->origin_axes = g_slist_append( model->origin_axes, point );
   } else {
      g_warning( "unable to allocate memory for origin axes point (%0.2f,%0.2f,%0.2f)\n", x, y, z );
   }
}



//**********************************************************
// Find the bounding box of this model 
// A.K.A.: sets xmin, xmax, ymin, ymax, zmin, zmax
// I should probably clean up some of the local variable glut here....
//
// Note: this resets worldspace so g3dav.c's CenterModel call may
// need to be recalled
void setBoundingBox(g3dav_model *model) {
   three_d_point *point;
   gfloat xmax, xmin;
   gfloat ymax, ymin;
   gfloat zmax, zmin;
   GSList *vertices;
   gdouble xrange, yrange, zrange, maxrange, xmidpoint, ymidpoint;
   gdouble abs_max;

   if (model->vertices==NULL) {
      return;
   }

   // get first point
   vertices = model->vertices;
   point = vertices->data;

   xmax = xmin = point->x;
   ymin = ymax = point->y;
   zmin = zmax = point->z;

   for (vertices=vertices->next; vertices; vertices=vertices->next) {

      point = vertices->data;

      if (point->x < xmin) xmin = point->x;
      if (point->x > xmax) xmax = point->x;
      if (point->y < ymin) ymin = point->y;
      if (point->y > ymax) ymax = point->y;
      if (point->z < zmin) zmin = point->z;
      if (point->z > zmax) zmax = point->z;
   }

   // any reason I'm buffering this local? I don't think so.
   model->xmax = xmax;
   model->xmin = xmin;
   model->ymax = ymax;
   model->ymin = ymin;
   model->zmax = zmax;
   model->zmin = zmin;

   // Set World space to be big enough to include all of object

      // make it square

      xrange = model->xmax - model->xmin;
      yrange = model->ymax - model->ymin;
      zrange = model->zmax - model->zmin;
      maxrange = max_of_3(xrange, yrange, zrange);
      //g_print( "xrange %f\n", xrange );
      //g_print( "yrange %f\n", yrange );
      //g_print( "zrange %f\n", zrange );
      //g_print( "maxrange %f\n", maxrange );
      maxrange *= 1.5; // widen the space a bit
      maxrange /= 2;   // divide here for convienence below

      //g_print( "maxrange %f\n", maxrange );
      //g_print( "model xmin: %f xmax: %f\n", model->xmin, model->xmax );
      //g_print( "model ymin: %f ymax: %f\n", model->ymin, model->ymax );

      xmidpoint = (model->xmax + model->xmin) / 2;
      ymidpoint = (model->ymax + model->ymin) / 2;
      //g_print( "x midpoint: %f\n", xmidpoint );
      //g_print( "y midpoint: %f\n", ymidpoint );

      model->w_xmin = (xmidpoint) - maxrange;
      model->w_xmax = (xmidpoint) + maxrange;

      model->w_ymin = (ymidpoint) - maxrange;
      model->w_ymax = (ymidpoint) + maxrange;

   //g_print( "world xmin: %f xmax: %f\n", model->w_xmin, model->w_xmax );
   //g_print( "world ymin: %f ymax: %f\n", model->w_ymin, model->w_ymax );

   // set up the origin axes here too
   if (model->origin_axes==NULL) {

      abs_max = fabs(model->xmin) > fabs(model->xmin) ? fabs(model->xmin) : fabs(model->xmax);
      add_origin_axes_point( model, -(abs_max*2), 0, 0 ); // x axes 1
      add_origin_axes_point( model, (abs_max*2), 0, 0 ); // x axes 2

      abs_max = fabs(model->ymin) > fabs(model->ymin) ? fabs(model->ymin) : fabs(model->ymax);
      add_origin_axes_point( model, 0, -(abs_max*2), 0 ); // y axes 1
      add_origin_axes_point( model, 0, (abs_max*2), 0 ); // y axes 2

      abs_max = fabs(model->zmin) > fabs(model->zmin) ? fabs(model->zmin) : fabs(model->zmax);
      add_origin_axes_point( model, 0, 0, -(abs_max*2) ); // z axes 1
      add_origin_axes_point( model, 0, 0, (abs_max*2) ); // z axes 2

   }

}


//**********************************************************
// calls appropriate loading method according the extension
gint load_model( g3dav_model* model, char* filename ) {
   gint rc=FALSE;
   char *dot;
   GString *gstr=NULL;

   dot = strrchr( filename, '.' );

   gstr = g_string_new( dot+1 );

   if (gstr) {
      g_string_up( gstr );

      if (strcmp( gstr->str, "OBJ" )==0) {
         rc = load_OBJ_Model( model, filename );
      } else if (strcmp( gstr->str, "PDB" )==0) {
         rc = load_PDB_Model( model, filename );
      } else if (strcmp( gstr->str, "CSV" )==0) {
         rc = load_CSV_Model( model, filename );
      }

      g_string_free( gstr, TRUE );
   }

   return rc;

}



//**********************************************************
// loads a Wavefront obj file
//   (modified to accept a text label)
gint load_OBJ_Model( g3dav_model* model, char* filename ) {
   FILE* infile=NULL;
   char buffer[128];
   char *token;

   double x, y, z;
   int    p1, p2;
   int    start, prev, n;

   infile = fopen( filename, "r" );
   if (!infile) {
      return (FALSE);
   }

   // NEED TO DO: add better numeric verification
   while ( fgets( buffer, sizeof(buffer), infile )) {

      //g_print( "read: '%s'\n", buffer );

      token = strtok( buffer, " " );
      if (token) {

         if (strcmp(token,"v")==0) {
            // vertex
            token = strtok( 0, " " );
            if (token) {
               x = atof( token );
               token = strtok( 0, " " );
               if (token) {
                  y = atof( token );
                  token = strtok( 0, " \n" );
                  if (token) {
                     z = atof( token );
                     addVertex( model, (gfloat)x, (gfloat)y, (gfloat)z );
                  }
               }
            }
            token = strtok( 0, " " );

         } else if (strcmp(token,"f")==0 || strcmp(token,"fo")==0 ) {
            // face 
            start=prev=n=-1;

            while (TRUE) {
               token = strtok( 0, " " );
               if (token) {
                  n = atoi(token);
                  if (prev>=0) {
                     addLine( model, prev, n );
                  }
                  if (start<0) {
                     start=n;
                  }
                  prev=n;
               } else {
                  break;
               }
            }
            if (start>=0) {
               // wraps back to complete face
               addLine( model, start, prev );
            }


         } else if (strcmp(token,"l")==0 ) {
            // line segment
            token = strtok( 0, " " );
            if (token) {
               p1 = atoi(token);
               token = strtok( 0, " " );
               if (token) {
                  p2 = atoi(token);
                  addLine( model, p1, p2 );
               }
            }
         } else if (strcmp(token,"t")==0) {
            // text label, format is  /t \d .+/
            token = strtok( 0, " " );
            if (token) {
               p1 = atoi(token);
               token = strtok( 0, " \n" );
               if (token) {
                  //label = (gchar*)g_free(sizeof(gchar)*strlen(token));
                  addLabel( model, p1, token );
               }
            }
         } // end line parsing by type

      } // end if

   } // end while file read 

   fclose( infile );

   setBoundingBox( model );

   if (model->nvert>0 && model->nline>0) {
      return (TRUE);
   } else {
      return (FALSE);
   }


}
// end load_OBJ_Model


//**********************************************************
// load a PDB molecule file
//   (modified to accept a text label)
gint load_PDB_Model( g3dav_model* model, char* filename ) {
   FILE* infile=NULL;
   char buffer[128];
   char temp[128];
   char *token;

   double x, y, z;
   int    p1;
   int    start, prev, n;

   infile = fopen( filename, "r" );
   if (!infile) {
      return (FALSE);
   }

   // NEED TO DO: add better numeric verification
   while ( fgets( buffer, sizeof(buffer), infile )) {

      //g_print( "read: '%s'\n", buffer );

      if (strncmp(buffer,"ATOM",4)==0) {

         strncpy( temp, &buffer[30], 8);
         temp[8]=0;
         x = atof( temp );

         strncpy( temp, &buffer[38], 8);
         temp[8]=0;
         y = atof( temp );

         strncpy( temp, &buffer[36], 8);
         temp[8]=0;
         z = atof( temp );

         p1 = addVertex( model, (gfloat)x, (gfloat)y, (gfloat)z );

         strncpy( temp, &buffer[12], 4);
         temp[4]=0;
         //label = g_strdup( temp );

         addLabel( model, p1, temp );

      } else if (strncmp(buffer,"CONECT",6)==0) {
         buffer[60]='\0';
         g_print( "CONECT\n" );

         token = strtok( buffer, " \n" ); // eat CONECT

         start=prev=n=-1;

         while (TRUE) {
            token = strtok( 0, " \n" );
            if (token) {
               n = atoi(token);
               g_assert(start!=0);
               if (start>0) {
                  // I'm guessing at 60 being a decent cut off 
                  g_print( "adding %d %d\n", start, n );
                  addLine( model, start, n );
               } else {
                  start=n;
               }
            } else {
               break;
            }
         }

      } // end CONNECT line

   } // end file read

   fclose( infile );

   setBoundingBox( model );

   //dumpModel(model);

   if (model->nvert>0) {
      return (TRUE);
   } else {
      return (FALSE);
   }


} // end load_PDB_Model




// **********************************************************
// loads a CSV (Comma Seperated Value) file
//   this is intended to be useful for plotting datasets
//   with three values.
//
//   no lines are drawn at this time, except for the axes,
//   just points in space
//
// this is a preliminary feature, needs work.
gint load_CSV_Model( g3dav_model* model, char* filename ) {
   FILE* infile=NULL;
   char buffer[128];
   char *token;
   gchar *label1=NULL, *label2=NULL, *label3=NULL;

   double x, y, z;
   //int    p1;
   int    n;

   infile = fopen( filename, "r" );
   if (!infile) {
      return (FALSE);
   }

   // NEED TO DO:  some sanity checking on the input data
   // read first line, should be column labels
   if (!fgets( buffer, sizeof(buffer), infile )) {
     return (FALSE);
   }
   if (strchr(buffer, ',')==NULL) {
     return (FALSE);
   }
   token = strtok( buffer, "," );
   if (token) {
      label1 = g_strdup( token );
      token = strtok( 0, "," );
      if (token) {
         label2 = g_strdup( token );
         token = strtok( 0, " \n" );
         if (token) {
            label3 = g_strdup( token );
         }
      }
   }

   // NEED TO DO: add better numeric verification
   n = 0;
   while ( fgets( buffer, sizeof(buffer), infile )) {

      if (strchr(buffer, ',')==NULL) {
         continue;
      }
      //g_print( "read: '%s'\n", buffer );

      token = strtok( buffer, "," );
      if (token) {
         x = atof( token );
         token = strtok( 0, "," );
         if (token) {
            y = atof( token );
            token = strtok( 0, " \n" );
            if (token) {
               z = atof( token );
               addVertex( model, (gfloat)x, (gfloat)y, (gfloat)z );
               n++;
               addLine( model, n, n );
            }
         }
      }
   } // end while file read 

   fclose( infile );

   setBoundingBox( model );

   addVertex( model, model->xmin, 0, 0 );
   addVertex( model, model->xmax, 0, 0 );
   addLine( model, ++n, ++n );
   addLabel( model, n, label1 );

   addVertex( model, 0, model->ymin, 0 );
   addVertex( model, 0, model->ymax, 0 );
   addLine( model, ++n, ++n );
   addLabel( model, n, label2 );

   addVertex( model, 0, 0, model->zmin );
   addVertex( model, 0, 0, model->zmax );
   addLine( model, ++n, ++n );
   addLabel( model, n, label3 );

   if (label1) {g_free(label1);}
   if (label2) {g_free(label2);}
   if (label3) {g_free(label3);}

   //dumpModel( model );

   // need to scale wider spread axes independently to 
   // achieve relative cubing.

   if (model->nvert>0 && model->nline>0) {
      return (TRUE);
   } else {
      return (FALSE);
   }
}
// end load_CSV_Model


