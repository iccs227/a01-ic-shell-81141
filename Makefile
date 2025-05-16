CC = gcc
CFLAGS = -Wall -g
BINARY = icsh

SRCS = icsh.c signals.c executor.c
OBJS = $(SRCS:.c=.o)

all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) -o $(BINARY) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm -f $(BINARY) *.o
