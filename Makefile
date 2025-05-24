CC       := gcc
CFLAGS   := -Wall -g -Iinclude
SRCDIR   := src
BUILDDIR := build
BINDIR   := bin
TARGET   := $(BINDIR)/icsh

SRCS     := $(wildcard $(SRCDIR)/*.c)
OBJS     := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

.PHONY: all clean directories

all: directories $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
	@ln -sf $(TARGET) ./icsh

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

directories:
	@mkdir -p $(BUILDDIR) $(BINDIR)
clean:
	rm -rf $(BUILDDIR)/*.o $(TARGET)
	rm -f ./icsh