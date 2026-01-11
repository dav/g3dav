# Generated automatically from Makefile.in by configure.
CC=gcc
SRCS=g3dav.c anaglyphic.c colordialog.c dialogs.c matrix3d.c model3d.c
OBJS=$(SRCS:.c=.o)
CCFLAGS=-I../include -D_DEBUG -g `gtk-config --cflags`
LIBS=`gtk-config --libs`


all: g3dav

g3dav: $(OBJS)
	$(CC) $(LDFLAGS) -o g3dav $(OBJS) $(LIBS)

$(OBJS): $(SRCS)
	$(CC) $(CCFLAGS) -c $(SRCS)

realclean: clean
	rm Makefile
.PHONY: realclean

clean:
	rm -f core $(OBJS) g3dav
.PHONY: clean

dep:
	makedepend -f Makefile -I../include $(SRCS)

