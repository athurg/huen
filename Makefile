CFLAGS+=-std=gnu99
CFLAGS+=`pkg-config json-glib-1.0 --cflags`
CFLAGS+=`pkg-config gtk+-2.0 --cflags`
LDFLAGS=-lcurl -export-dynamic
LDFLAGS+=`pkg-config json-glib-1.0 --libs`
LDFLAGS+=`pkg-config gtk+-2.0 --libs`

OBJS=huen.o download.o parse.o callback.o

all:huen

huen:${OBJS}

clean:
	-@rm -rf *.o

distclean:clean
	-@rm -rf huen

rebuild:distclean huen
