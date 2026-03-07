CC ?= gcc

# Build mode: 'debug' or 'release'
BUILD ?= debug

BINDIR = bin
OBJDIR = obj

TARGET = $(BINDIR)/webtest

SOURCES = main.c	\
		  webtest.c	\

OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))

CFLAGS ?=

ifeq ($(BUILD), debug)
	CFLAGS += -Wall -g
else
	ifeq ($(BUILD), release)
		CFLAGS += -O2
	endif
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJDIR)

distclean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean distclean
