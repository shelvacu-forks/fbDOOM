##########################################################
#
# $Id:$
#
# $Log:$
#
#

CC ?= gcc
CFLAGS += -O -DNORMALUNIX -DLINUX -std=c11
LDFLAGS +=
LIBS += -lm

OUTPUT=sndserver

all: $(OUTPUT)
.PHONY: all

clean:
	rm -f *.o *~ *.flc sndserver
.PHONY: clean

OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))

$(OUTPUT): $(OBJECTS)
	@echo "[linking $@]"
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)
	echo make complete.

%.o: %.c
	@echo "[compiling $<]"
	$(CC) $(CFLAGS) -c $< -o $@


