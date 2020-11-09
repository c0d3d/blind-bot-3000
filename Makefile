
SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)

LDFLAGS := -Wall
LDLIBS  := -lm
CFLAGS  := -Iinclude -Wall

all: bb3000

bb3000: $(OBJS)
	$(CC) $(LDFLAGS) -o bb3000 $^ -lm

clean:
	rm -rf $(OBJS) bb3000

.PHONY: all clean
