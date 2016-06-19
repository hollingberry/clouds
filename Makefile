main: main.c
	clang -o $@ $< -lGLEW -lglfw3 -framework OpenGL
