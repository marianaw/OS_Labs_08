# $Rev: 532 $
TARGET = main
#DEBUG  = debug
CC = gcc
CFLAGS = -Wall -Wextra -Wdeclaration-after-statement -Wbad-function-cast \
         -Wcast-qual -Wstrict-prototypes -Wmissing-declarations \
         -Wmissing-prototypes -Werror -g  `pkg-config --cflags glib-2.0` -D_GNU_SOURCE
#DFLAGS = -D__DEBUG
LDFLAGS=`pkg-config --libs glib-2.0` 
LIBS = -lpthread -lm 
SOURCES = $(shell echo *.c)
OBJECTS = $(SOURCES:%.c=%.o)

all: $(TARGET)

#debug: $(DEBUG)

#$(DEBUG) : $(OBJECTS)
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS) $(DFLAGS)
	

test: $(OBJECTS)
	make -C tests test

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

-include .depend

.depend: $(SOURCES) *.h
	$(CC) -MM $(SOURCES) > .depend

.PHONY: clean

clean:
	make -C tests clean
	rm -f $(TARGET) $(OBJECTS) .depend *~
