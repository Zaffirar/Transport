CC = gcc
CFLAGS =  -std=gnu99 -Wextra -Wall -Wno-unused-parameter -Werror #-g3 -fsanitize=address

TARGET = transport

SRCS = slidingWindow.c socketAndServerHandling.c main.c
OBJS = slidingWindow.o socketAndServerHandling.o main.o

.PHONY: all clean distclean

all = $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

clean:
		$(RM) $(OBJS) 

distclean:
		$(RM) $(OBJS) $(TARGET)