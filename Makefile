
CC = g++
SDIR = ./src
OPTIMIZE = -DNDEBUG -Os -fstrict-enums -flto -pipe
DEBUG = -DDEBUG -g -Wall -Og
CFLAGS = -std=c++11
LIBS = -lncurses

NAME = SlicerSnake.exe

release: CFLAGS += $(OPTIMIZE)
release: SlicerSnake
	strip --strip-debug --strip-unneeded --preserve-dates $(NAME)

debug: CFLAGS += $(DEBUG)
debug: SlicerSnake

SlicerSnake: $(SDIR)/main.cpp display.o game.o snake.o input.o
	$(CC) $(CFLAGS) $(SDIR)/main.cpp display.o game.o snake.o input.o -o $(NAME) $(LIBS)

snake.o: $(SDIR)/snake.h $(SDIR)/snake.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/snake.cpp

display.o: $(SDIR)/display.h $(SDIR)/display.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/display.cpp

game.o: $(SDIR)/game.h $(SDIR)/game.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/game.cpp

input.o: $(SDIR)/input.h $(SDIR)/input.cpp
	$(CC) $(CFLAGS) -c $(SDIR)/input.cpp

clean:
	rm $(NAME) *.o
