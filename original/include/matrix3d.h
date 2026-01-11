#ifndef _G3DAV_MATRIX3D
#define _G3DAV_MATRIX3D

#include <gtk/gtk.h>

#define PI 3.1415926535

typedef struct g3dav_matrix {
   gfloat xx, xy, xz, xo;
   gfloat yx, yy, yz, yo;
   gfloat zx, zy, zz, zo;
} g3dav_matrix;

g3dav_matrix* g3dav_matrix_new();
void g3dav_matrix_destroy(g3dav_matrix *matrix);

/** Scale uniformaly */
void uniform_scale(g3dav_matrix* matrix, gfloat f);

/** Scale along each axis independently */
void independent_scale(g3dav_matrix* matrix, gfloat xf, gfloat yf, gfloat zf);

/** Translate the origin */
void translate(g3dav_matrix* matrix, gfloat x, gfloat y, gfloat z);

/** rotate theta degrees about the y axis */
void yrot(g3dav_matrix* matrix, gdouble theta);

/** rotate theta degrees about the x axis */
void xrot(g3dav_matrix* matrix, gdouble theta);

/** rotate theta degrees about the z axis */
void zrot(g3dav_matrix* matrix, gdouble theta);

/** Multiply this matrix by a second: M = M*R */
void multiplyBy(g3dav_matrix* m1, g3dav_matrix* rhs);

/** Reinitialize to the unit matrix */
void unit(g3dav_matrix* matrix );

/** Transform nvert points from v into tv.  v contains the input
    coordinates in gfloating point.  Three successive entries in
    the array constitute a point.  tv ends up holding the transformed
    points as gdoubles; three successive entries per point */
//void transform(gfloat v[], gdouble tv[], int nvert);
GSList *transform(g3dav_matrix* matrix, GSList *modelVertices);

#endif _G3DAV_MATRIX3D

