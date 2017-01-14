# $Rev: 530 $
# You should run 'make test' in parent directory
TARGET = runner
CC = gcc
CFLAGS += -I.. -ansi -Wall -Wextra -Wdeclaration-after-statement \
          -Wbad-function-cast -Wcast-qual -Wstrict-prototypes \
          -Wmissing-declarations -Wmissing-prototypes \
          -Wno-unused-parameter -Werror `pkg-config --cflags glib-2.0`
LGFLAGS=`pkg-config --libs glib-2.0`
LDFLAGS = -lcheck -lm -lpthread
SOURCES = $(shell echo *.c)
OBJECTS = $(SOURCES:.c=.o)
TESTED_OBJECTS = $(shell ls ../*.o | grep -v main)

all: $(TARGET)

test: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJECTS) $(TESTED_OBJECTS)
	$(CC) $(LGFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJECTS) .depend *~ test.log

.depend: *.[ch]
	$(CC) $(CFLAGS) -MM $(SOURCES) >.depend

-include .depend

.PHONY: clean all

