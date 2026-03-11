CC ?= gcc

# Build mode: 'debug' or 'release'
BUILD ?= debug

BINDIR = bin
OBJDIR = obj

TARGET = $(BINDIR)/webtest

VPATH = core	\
		http

SOURCES = main.c			\
		  webtest_core.c	\
		  http.c
		  
HEADERS = webtest_core.h	\
		  http.h			\
		  error.h			\
		  index.html.h

OBJECTS = $(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))

CFLAGS ?=

ifeq ($(BUILD), debug)
	CFLAGS += -Wall -g
else
	ifeq ($(BUILD), release)
		CFLAGS += -O2 -s
	endif
endif

all: $(TARGET)

$(OBJECTS): $(HEADERS)

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
