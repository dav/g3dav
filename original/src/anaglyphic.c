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
#include "anaglyphic.h"

g3dav_anaglyphic* g3dav_ana_new() {
  g3dav_anaglyphic* anaglyphic;

  anaglyphic = g_malloc(sizeof(g3dav_anaglyphic));
  if (anaglyphic) {
    anaglyphic->view_dis=50.0; 
    anaglyphic->eye_dis=5.0;
    anaglyphic->red_x=0;
    anaglyphic->red_y=0;
    anaglyphic->cyan_x=0;
    anaglyphic->cyan_y=0;
  }

  return anaglyphic;

}

void g3dav_ana_translate( g3dav_anaglyphic* anag, gdouble x, gdouble y, gdouble z ) {

  anag->red_y = anag->cyan_y = anag->view_dis*y / (anag->view_dis-z);

  anag->red_x = (anag->view_dis*x - anag->eye_dis*z/2) 
                / (anag->view_dis-z);

  anag->cyan_x = (anag->view_dis*x + anag->eye_dis*z/2) 
                / (anag->view_dis-z);

  //g_print( "anaglyphic: (%f, %f, %f)\n=> (%f, %f) & (%f, %f)\n", x, y, z, anag->red_x, anag->red_y, anag->cyan_x, anag->cyan_y );
}


