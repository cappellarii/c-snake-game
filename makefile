CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -g
LDLIBS = -lglut -lGL -lGLU -lm
WINDOWS_LDLIBS = -lfreeglut -lopengl32 -lglu32

ifeq ($(OS),Windows_NT)
WINDOWS_CC = $(CC)
DEFAULT_TARGET = snake-game.exe
else
WINDOWS_CC = x86_64-w64-mingw32-gcc
DEFAULT_TARGET = snake-game
endif

.PHONY: all windows clean

all: $(DEFAULT_TARGET)

windows: snake-game.exe

snake-game: main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

snake-game.exe: main.c
	$(WINDOWS_CC) $(CPPFLAGS) $(WINDOWS_CPPFLAGS) $(CFLAGS) $(LDFLAGS) $(WINDOWS_LDFLAGS) -o $@ $^ $(WINDOWS_LDLIBS)

clean:
	$(RM) snake-game snake-game.exe