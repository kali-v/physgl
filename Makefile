all:
	g++ -ggdb -g src/main.cpp src/glad.c -lglfw3 -lm -lX11 -ldl -lpthread -o main
run:
	g++ src/main.cpp src/glad.c -lglfw3 -lm -lX11 -ldl -lpthread -o main
	./main
