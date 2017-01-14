# $Date: 2008-10-31 09:14:09 -0200 (Fri, 31 Oct 2008) $
# $Revision: 682 $

TARGET=fusewrapper
CC=gcc
# -Wextra: no me deja comparar signed con unsigned y esto es bastante común con size_t y off_t
# -pedantic: no deja inicializar la estructura fuse_operations
CFLAGS+= -ansi -Werror -Wall -Wdeclaration-after-statement -Wbad-function-cast -Wcast-qual -Wstrict-prototypes -Wmissing-declarations  -Wmissing-prototypes -Wno-unused-parameter -g `pkg-config --cflags fuse` `pkg-config --cflags glib-2.0`
LDFLAGS=`pkg-config --libs fuse` `pkg-config --libs glib-2.0`

SOURCES=$(shell echo *.c)
OBJECTS=$(SOURCES:.c=.o)
BSTRING_OBJECTS=bstring/bstrlib.o

all: $(TARGET)

# BString requiere flags distintos:
$(BSTRING_OBJECTS):CFLAGS=-ansi -Wall -Werror -pedantic -g

$(TARGET): $(OBJECTS) $(BSTRING_OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS) $(BSTRING_OBJECTS) .depend *~
	
.depend: *.[ch]
	$(CC) -MM `pkg-config fuse --cflags` `pkg-config --cflags glib-2.0` $(SOURCES) >.depend

-include .depend

.PHONY: clean all
