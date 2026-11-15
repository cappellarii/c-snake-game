CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g
LDLIBS = -lglut -lGL -lGLU -lm

.PHONY: all clean

all: snake-game

snake-game: main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	$(RM) snake-game