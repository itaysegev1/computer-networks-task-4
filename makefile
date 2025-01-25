CC=gcc
CFLAGS=-Wall -Wextra
LDFLAGS=-lm

TARGETS=ping tracerout

all: $(TARGETS)

ping: ping.o
   $(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

tracerout: tracerout.o
   $(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

%.o: %.c
   $(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
   rm -f *.o $(TARGETS)
