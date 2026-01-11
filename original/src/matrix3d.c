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

#include <math.h>

#include "matrix3d.h"
#include "model3d.h"

// quiver of functions to perform on a g3dav_matrix object.
// oops, i mean structure.

// also a special function to transform a list of vertices
// against a given matrix.

g3dav_matrix* g3dav_matrix_new() {
  g3dav_matrix *m;

  m =  g_malloc(sizeof(g3dav_matrix));
  if (m) {
    unit(m);
  }

  return m;
}

// ********************************************************************
// say no to memory leaks
void g3dav_matrix_destroy(g3dav_matrix *matrix) {
   // destroy the model
   g_free(matrix);
}

/** Scale by f in all dimensions */
void uniform_scale(g3dav_matrix* matrix, gfloat f) {
  matrix->xx *= f;
  matrix->xy *= f;
  matrix->xz *= f;
  matrix->xo *= f;
  matrix->yx *= f;
  matrix->yy *= f;
  matrix->yz *= f;
  matrix->yo *= f;
  matrix->zx *= f;
  matrix->zy *= f;
  matrix->zz *= f;
  matrix->zo *= f;
}

/** Scale along each axis independently */
void independent_scale(g3dav_matrix* matrix, gfloat xf, gfloat yf, gfloat zf) {
  matrix->xx *= xf;
  matrix->xy *= xf;
  matrix->xz *= xf;
  matrix->xo *= xf;
  matrix->yx *= yf;
  matrix->yy *= yf;
  matrix->yz *= yf;
  matrix->yo *= yf;
  matrix->zx *= zf;
  matrix->zy *= zf;
  matrix->zz *= zf;
  matrix->zo *= zf;
}

/** Translate the origin */
void translate(g3dav_matrix* matrix, gfloat x, gfloat y, gfloat z) {
  matrix->xo += x;
  matrix->yo += y;
  matrix->zo += z;
}

/** rotate theta degrees about the y axis */
void yrot(g3dav_matrix* matrix, gdouble theta) {
  gfloat Nxx,Nxy,Nxz,Nxo;
  gfloat Nzx,Nzy,Nzz,Nzo;
  gdouble ct, st;

  theta *= (PI / 180);

  ct = cos(theta);
  st = sin(theta);

  Nxx = (gfloat) (matrix->xx * ct + matrix->zx * st);
  Nxy = (gfloat) (matrix->xy * ct + matrix->zy * st);
  Nxz = (gfloat) (matrix->xz * ct + matrix->zz * st);
  Nxo = (gfloat) (matrix->xo * ct + matrix->zo * st);

  Nzx = (gfloat) (matrix->zx * ct - matrix->xx * st);
  Nzy = (gfloat) (matrix->zy * ct - matrix->xy * st);
  Nzz = (gfloat) (matrix->zz * ct - matrix->xz * st);
  Nzo = (gfloat) (matrix->zo * ct - matrix->xo * st);

  matrix->xo = Nxo;
  matrix->xx = Nxx;
  matrix->xy = Nxy;
  matrix->xz = Nxz;
  matrix->zo = Nzo;
  matrix->zx = Nzx;
  matrix->zy = Nzy;
  matrix->zz = Nzz;
}

/** rotate theta degrees about the x axis */
void xrot(g3dav_matrix* matrix, gdouble theta) {
  gfloat Nyx,Nyy,Nyz,Nyo;
  gfloat Nzx,Nzy,Nzz,Nzo;
  gdouble ct, st;

  theta *= (PI / 180);
  ct = cos(theta);
  st = sin(theta);

  Nyx = (gfloat) (matrix->yx * ct + matrix->zx * st);
  Nyy = (gfloat) (matrix->yy * ct + matrix->zy * st);
  Nyz = (gfloat) (matrix->yz * ct + matrix->zz * st);
  Nyo = (gfloat) (matrix->yo * ct + matrix->zo * st);

  Nzx = (gfloat) (matrix->zx * ct - matrix->yx * st);
  Nzy = (gfloat) (matrix->zy * ct - matrix->yy * st);
  Nzz = (gfloat) (matrix->zz * ct - matrix->yz * st);
  Nzo = (gfloat) (matrix->zo * ct - matrix->yo * st);

  matrix->yo = Nyo;
  matrix->yx = Nyx;
  matrix->yy = Nyy;
  matrix->yz = Nyz;
  matrix->zo = Nzo;
  matrix->zx = Nzx;
  matrix->zy = Nzy;
  matrix->zz = Nzz;
}

/** rotate theta degrees about the z axis */
void zrot(g3dav_matrix* matrix, gdouble theta) {
  gfloat Nxx,Nxy,Nxz,Nxo;
  gfloat Nyx,Nyy,Nyz,Nyo;
  gdouble ct, st;

  theta *= (PI / 180);
  ct = cos(theta);
  st = sin(theta);

  Nyx = (gfloat) (matrix->yx * ct + matrix->xx * st);
  Nyy = (gfloat) (matrix->yy * ct + matrix->xy * st);
  Nyz = (gfloat) (matrix->yz * ct + matrix->xz * st);
  Nyo = (gfloat) (matrix->yo * ct + matrix->xo * st);

  Nxx = (gfloat) (matrix->xx * ct - matrix->yx * st);
  Nxy = (gfloat) (matrix->xy * ct - matrix->yy * st);
  Nxz = (gfloat) (matrix->xz * ct - matrix->yz * st);
  Nxo = (gfloat) (matrix->xo * ct - matrix->yo * st);

  matrix->yo = Nyo;
  matrix->yx = Nyx;
  matrix->yy = Nyy;
  matrix->yz = Nyz;
  matrix->xo = Nxo;
  matrix->xx = Nxx;
  matrix->xy = Nxy;
  matrix->xz = Nxz;
}

// Multiply this matrix by a second: M = M*R 
void multiplyBy(g3dav_matrix* m1, g3dav_matrix* rhs) {
  gfloat lxx = m1->xx * rhs->xx + m1->yx * rhs->xy + m1->zx * rhs->xz;
  gfloat lxy = m1->xy * rhs->xx + m1->yy * rhs->xy + m1->zy * rhs->xz;
  gfloat lxz = m1->xz * rhs->xx + m1->yz * rhs->xy + m1->zz * rhs->xz;
  gfloat lxo = m1->xo * rhs->xx + m1->yo * rhs->xy + m1->zo * rhs->xz + rhs->xo;

  gfloat lyx = m1->xx * rhs->yx + m1->yx * rhs->yy + m1->zx * rhs->yz;
  gfloat lyy = m1->xy * rhs->yx + m1->yy * rhs->yy + m1->zy * rhs->yz;
  gfloat lyz = m1->xz * rhs->yx + m1->yz * rhs->yy + m1->zz * rhs->yz;
  gfloat lyo = m1->xo * rhs->yx + m1->yo * rhs->yy + m1->zo * rhs->yz + rhs->yo;

  gfloat lzx = m1->xx * rhs->zx + m1->yx * rhs->zy + m1->zx * rhs->zz;
  gfloat lzy = m1->xy * rhs->zx + m1->yy * rhs->zy + m1->zy * rhs->zz;
  gfloat lzz = m1->xz * rhs->zx + m1->yz * rhs->zy + m1->zz * rhs->zz;
  gfloat lzo = m1->xo * rhs->zx + m1->yo * rhs->zy + m1->zo * rhs->zz + rhs->zo;

  m1->xx = lxx;
  m1->xy = lxy;
  m1->xz = lxz;
  m1->xo = lxo;

  m1->yx = lyx;
  m1->yy = lyy;
  m1->yz = lyz;
  m1->yo = lyo;

  m1->zx = lzx;
  m1->zy = lzy;
  m1->zz = lzz;
  m1->zo = lzo;
}

/** Reinitialize to the unit matrix */
void unit(g3dav_matrix* matrix) {
  matrix->xo = 0;
  matrix->xx = 1;
  matrix->xy = 0;
  matrix->xz = 0;
  matrix->yo = 0;
  matrix->yx = 0;
  matrix->yy = 1;
  matrix->yz = 0;
  matrix->zo = 0;
  matrix->zx = 0;
  matrix->zy = 0;
  matrix->zz = 1;
}

// maybe this should go in another source file
/** Transform nvert points from v into tv.  v contains the input
    coordinates in gfloating point.  Three successive entries in
    the array constitute a point.  tv ends up holding the transformed
    points as gdoubles; three successive entries per point */
GSList *transform(g3dav_matrix* matrix, GSList *modelVertices) {
  
  GSList *node;
  three_d_point *old_point;
  three_d_point *new_point;

  gfloat lxx = matrix->xx, lxy = matrix->xy, lxz = matrix->xz, lxo = matrix->xo;
  gfloat lyx = matrix->yx, lyy = matrix->yy, lyz = matrix->yz, lyo = matrix->yo;
  gfloat lzx = matrix->zx, lzy = matrix->zy, lzz = matrix->zz, lzo = matrix->zo;

  GSList *tmodel = NULL;

  for (node = modelVertices; node; node = node->next) {
    old_point = node->data;
    new_point = g_malloc(sizeof(three_d_point));
    if (new_point) {
      new_point->x = (gdouble) (old_point->x * lxx + 
                                old_point->y * lxy + 
                                old_point->z * lxz + lxo);

      new_point->y = (gdouble) (old_point->x * lyx + 
                                old_point->y * lyy + 
                                old_point->z * lyz + lyo);

      new_point->z = (gdouble) (old_point->x * lzx + 
                                old_point->y * lzy + 
                                old_point->z * lzz + lzo);

      //g_print( "adding (%0.2f,%0.2f,%0.2f)\n", point->x, point->y, point->z );

      tmodel = g_slist_append( tmodel, new_point );
    } else {
      g_warning( "unable to allocate memory for new point (%0.2f,%0.2f,%0.2f)\n", 
                 new_point->x, new_point->y, new_point->z );
    }          
  }
  return tmodel;
}


