all:
	g++ src/main.cpp src/geometry/line.cpp src/geometry/coordinate.cpp src/rigid_body.cpp src/glad.c -lglfw3 -lm -O2 -o physgl
run:
	g++ src/main.cpp src/geometry/line.cpp src/geometry/coordinate.cpp src/rigid_body.cpp src/glad.c -lglfw3 -lm -O2 -o physgl && ./physgl
debug:
	g++ -ggdb -g src/main.cpp src/geometry/line.cpp src/geometry/coordinate.cpp src/rigid_body.cpp src/glad.c -lglfw3 -lm -lX11 -ldl -lpthread -o physgl