CC = gcc

CPPFLAGS = -MMD -Wall -Wextra -Werror
CFLAGS = -std=gnu99 -O2 -fomit-frame-pointer
LDFLAGS = -Wl,-O2


SRCS = slidingWindow.c socketAndServerHandling.c transport.c
OBJS = $(subst .c,.o, $(SRCS)) transport
DEPS = $(subst .c,.d, $(SRCS))

.PHONY: all clean distclean

all: transport

transport: slidingWindow.o socketAndServerHandling.o transport.o

clean:
	rm -f *.o *.d

distclean: clean
	rm -f transport

-include $(DEPS)
