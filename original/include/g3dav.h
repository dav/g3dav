typedef struct {
   gint rx1, ry1, cx1, cy1;
   gint rx2, ry2, cx2, cy2;
} anaglyphic_line;

GtkWidget *CreateWidgetFromXpm( GtkWidget *window, gchar **xpm_data);
void CenterModel(g3dav_model* _model, g3dav_matrix* _matrix);
void make_anaglyphic( three_d_point* p1, three_d_point* p2 );



static const gchar *xpm_open[] = {
   " 16 16 4 1",           // 16 x 16, 4 colors, 1char per color
   "  c None",             // color 1 is transparent
   "B c #000000000000",
   "Y c #FFFFFFFF0000",
   "y c #999999990000",
   "                ",
   "          BBB   ",
   "   BBBB  B   BB ",
   "   BYYB      BB ",
   " BYYYYBBBBBB    ",
   " BYYYYYYYYYB    ",
   " BYYYYYYYYYB    ",
   " BYYYYYYYYYB    ",
   " BBYYBBBBBBBBBB ",
   " BYYByyyyyyyyyB ",
   " BYByyyyyyyyyB  ",
   " BYByyyyyyyyyB  ",
   " BByyyyyyyyyB   ",
   " BByyyyyyyyyB   ",
   " BBBBBBBBBBB    ",
   "                "
};
static const char *xpm_rgb[] = {
"16 16 4 1",
"  c None",
"R c #FF0000",
"G c #00FF00",
"B c #0000FF",
"                ",
"     BBBRRR     ",
"   BBBBBRRRRR   ",
"  BBBBBBRRRRRR  ",
"  BBBBBBRRRRRR  ",
"  BBBBBBRRRRRR  ",
" BBBBBBBRRRRRRR ",
" BBBBBBBRRRRRRR ",
" BBBBBGGGGRRRRR ",
" BBBGGGGGGGGRRR ",
"  GGGGGGGGGGGG  ",
"  GGGGGGGGGGGG  ",
"  GGGGGGGGGGGG  ",
"   GGGGGGGGGG   ",
"     GGGGGGG    ",
"                ",
};

static char * xpm_save[] = {
"16 16 4 1",
" 	c None",
".	c #000000",
"+	c #00FF00",
"@	c #C9CCC9",
"                ",
"  ............  ",
" .++.@@@++@.++. ",
" .++.@@@++@.++. ",
" .++.@@@++@.++. ",
" .++.@@@@@@.++. ",
" .++........++. ",
" .++++++++++++. ",
" .++++++++++++. ",
" .++++++++++++. ",
" .++++++++++++. ",
" .++++++++++++. ",
" .+.++++++++.+. ",
" .++++++++++++. ",
"  ............  ",
"                "};



/* XPM */
static char * xpm_prev[] = {
"16 16 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"                ",
"                ",
"                ",
"    ..          ",
"   ...          ",
"  ..+.          ",
" ..++.......... ",
" .++++++++++++. ",
" ..+++++++++++. ",
"  ..+.......... ",
"   ...          ",
"    ..          ",
"                ",
"                ",
"                ",
"                "};


/* XPM */
static char * xpm_next[] = {
"16 16 3 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"                ",
"                ",
"                ",
"          ..    ",
"          ...   ",
"          .+..  ",
" ..........++.. ",
" .++++++++++++. ",
" .+++++++++++.. ",
" ..........+..  ",
"          ...   ",
"          ..    ",
"                ",
"                ",
"                ",
"                "};

/* XPM */
static char * xpm_3d[] = {
"16 16 5 1",
" 	c None",
".	c #000000",
"+	c #FFFFFF",
"@	c #FF0000",
"#	c #00FFFF",
"                ",
"                ",
"                ",
"  ............  ",
" .++++++++++++. ",
" .+@@@@++####+. ",
" .+@@@@++####+. ",
" .+@@@+..+###+. ",
" .++++.  .++++. ",
"  .....  ...... ",
"                ",
"                ",
"                ",
"                ",
"                ",
"                "};

/* XPM */
static char * xpm_square[] = {
"16 16 5 1",
" 	c None",
".	c #000000",
"+	c #FF0000",
"@	c #00FFFF",
"#	c #FFFFFF",
"                ",
" .........      ",
" ......... .    ",
" .+@...... . .  ",
" ..###+... . . .",
" ..#.@.+.. . . .",
" ..@+.@... . .  ",
" ..@.+.@.. .    ",
" ......... .    ",
" .........      ",
"                ",
"   .....        ",
"                ",
"     .          ",
"                ",
"                "};


static char * xpm_zoomin[] = {
"16 16 3 1",
/* colors */
"       c None",
".      c black",
"X      c white",
/* pixels */
"                ",
"                ",
"   ....         ",
"  .    .    ..  ",
" .  X   .   ..  ",
" . X    . ......",
" .      . ......",
" .      .   ..  ",
"  .    ..   ..  ",
"   .......      ",
"        ...     ",
"         ...    ",
"          ...   ",
"           ..   ",
"                ",
"                "};

static char * xpm_zoomout[] = {
"16 16 3 1",
/* colors */
"       c None",
".      c black",
"X      c white",
/* pixels */
"                ",
"                ",
"   ....         ",
"  .    .        ",
" .  X   .       ",
" . X    .  .....",
" .      .  .....",
" .      .       ",
"  .    ..       ",
"   .......      ",
"        ...     ",
"         ...    ",
"          ...   ",
"           ..   ",
"                ",
"                "};

static char * xpm_default[] = {
"16 16 4 1",
/* colors */
"       c None",
".      c black",
"X      c white",
"r      c red",
/* pixels */
"                ",
"                ",
"   ....         ",
"  .    .        ",
" .  X   .       ",
" .rr    . rr    ",
" . rr   .rr     ",
" .  rr  rr      ",
"  .  rrrr       ",
"   ...rr..      ",
"     rrrr..     ",
"    rr  rr..    ",
"   rr    rr..   ",
"  rr      rr.   ",
"                ",
"                "};

