CC      ?= gcc
CFLAGS  := -Wall -Wextra -g -std=c99 -D_POSIX_C_SOURCE=200809L
TARGET  := wvd

# Windows builds need the .exe suffix
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
endif

SRCS := main.c $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

# Memory check, Linux only
check: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

clean:
	rm -f $(OBJS) wvd wvd.exe

.PHONY: all run check clean
