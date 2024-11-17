CC=gcc
CFLAGS=-g -std=c99 -Wall -Werror -pedantic -O3 

SRC_FILES=c/main.c c/camera.c c/obj.c c/texture.c c/vec3.h

all: $(SRC_FILES) 
	$(CC) $(CFLAGS) -o main $(SRC_FILES) -lm -lpng
