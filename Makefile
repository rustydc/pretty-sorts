GL_INCLUDE = /usr/X11R6/include
GL_LIB = /usr/X11R6/lib

gl: gl.o sort.o
	gcc -g -Wall -o gl $^ -L$(GL_LIB) -lGL -lglut -lGLEW -std=c99 -g

heap: heap.o
	gcc -g -Wall -o heap $^ -L$(GL_LIB) -lm -std=c99 -g

.c.o:
	gcc -g -Wall -c -o $@ $< -I$(GL_INCLUDE) -lglut -std=c99 -g

clean:
	rm -f sort.o gl.o gl heap.o heap
