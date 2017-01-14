TARGET=baash
CC=gcc
CFLAGS+= -ansi -Wall -pedantic -g `pkg-config --cflags glib-2.0`
LDFLAGS=`pkg-config --libs glib-2.0`
SOURCES=$(shell echo *.c)
BSTRING_OBJECTS=bstring/bstrlib.o
SOURCES+=lexer/lexer.c
OBJECTS=$(SOURCES:.c=.o)

all: $(TARGET)

# BString requiere flags distintos:
$(BSTRING_OBJECTS):CFLAGS=-ansi -Wall -Werror -pedantic -g

$(TARGET): $(OBJECTS) $(BSTRING_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

clean:
	rm -f $(TARGET) $(OBJECTS) .depend *~
	make -C tests clean

test: $(OBJECTS)
	make -C tests test

.depend: *.[ch]
	$(CC) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all
