CC := g++
CFLAGS := -O2 -lglfw3 -lm
DEBUG_CFLAGS := -lglfw3 -lm -lX11 -ldl -lpthread -ggdb -g
SRC := src/main.cpp src/geometry/line.cpp src/geometry/coordinate.cpp src/rigid_body.cpp src/glad.c

all: physgl

physgl:
	$(CC) $(SRC) $(CFLAGS) -o physgl

run: clean physgl
	./physgl

debug:
	g++ $(SRC) $(DEBUG_CFLAGS) -o physgl

clean:
	rm -f physgl