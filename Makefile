
SRCS := $(wildcard src/*.c)
INCLUDES := $(wildcard include/*.h)
OBJS := $(SRCS:.c=.o)

LDFLAGS := -Wall
LDLIBS  := -lm
CFLAGS  := -Iinclude -Wall

all: bb3000

bb3000: $(OBJS) $(INCLUDES)
	$(CC) $(LDFLAGS) -o bb3000 $^ -lm

clean:
	rm -rf $(OBJS) bb3000

.PHONY: all clean
