CC = g++
ifeq ($(shell sw_vers 2>/dev/null | grep macOS | awk '{ print $$2}'),macOS)
CFLAGS = -g -Wall
INCFLAGS = -I./glm-0.9.7.1 -I/usr/X11/include -I./include/
LDFLAGS = -L/opt/homebrew/lib -lGLEW -lfreeimage -L/opt/homebrew/Cellar/freeglut/3.4.0/lib -framework GLUT 
else
CFLAGS = -g -DGL_GLEXT_PROTOTYPES
INCFLAGS = -I./glm-0.9.7.1 -I./include/ -I/usr/X11R6/include -I/sw/include \
		-I/usr/sww/include -I/usr/sww/pkg/Mesa/include
LDFLAGS = -lGLEW -framework GLUT -lGLU -lGL -lX11 -lfreeimage
endif

RM = /bin/rm -f
all: transforms
transforms: main.o Transform.o Raytrace.o readfile.o variables.h readfile.h Raytrace.h Transform.h \
		Geometry.h 
	$(CC) $(CFLAGS) -o transforms main.o Transform.o readfile.o Raytrace.o $(INCFLAGS) $(LDFLAGS)
main.o: main.cpp Transform.h readfile.h variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c main.cpp
readfile.o: readfile.cpp readfile.h variables.h Geometry.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c readfile.cpp
Raytrace.o: Raytrace.cpp Geometry.h Transform.h readfile.h variables.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Raytrace.cpp
Transform.o: Transform.cpp Transform.h
	$(CC) $(CFLAGS) $(INCFLAGS) -c Transform.cpp
clean:
	$(RM) *.o transforms *.png


 
