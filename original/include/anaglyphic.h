#ifndef _G3DAV_ANAGLYPHIC
#define _G3DAV_ANAGLYPHIC

typedef struct g3dav_anaglyphic {
  gdouble view_dis, eye_dis;
  gdouble red_x, red_y, cyan_x, cyan_y;
} g3dav_anaglyphic;

g3dav_anaglyphic* g3dav_ana_new();

void g3dav_ana_translate( g3dav_anaglyphic* anag, gdouble x, gdouble y, gdouble z );


#endif
