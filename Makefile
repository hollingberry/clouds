all: main

main: src/main.c
	clang -o $@ $< -Iinclude -lGLEW -lglfw3 -framework OpenGL
