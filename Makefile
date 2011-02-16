CFLAGS+=-std=gnu99
CFLAGS+=`pkg-config json-glib-1.0 --cflags`
CFLAGS+=`pkg-config gtk+-2.0 --cflags`
LDFLAGS=-lfetch
LDFLAGS+=`pkg-config json-glib-1.0 --libs`
LDFLAGS+=`pkg-config gtk+-2.0 --libs`

OBJS=huen.o download.o parse.o

huen:${OBJS}
	$(CC) $(LDFLAGS) ${OBJS} -o $@
clean:
	-@rm -rf *.o

distclean:clean
	-@rm -rf huen
