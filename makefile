# Cartocraft Makefile
# Copyright (C) 2012 David Jolly

CC=g++
HEADERS=lib/headers/
LIB=lib/
LODE=src/lode/
OUT=cartocraft
SRC=src/
FLAGS=-std=c++0x -lboost_regex -lboost_filesystem -lz -I $(HEADERS) -L $(LIB) -lanvil -O3 -funroll-all-loops

all: build carto

build: block_color.o image_buffer.o lodepng.o

carto: build $(SRC)carto.cpp $(SRC)carto.hpp
	$(CC) -o $(OUT) $(SRC)carto.cpp $(SRC)block_color.o $(SRC)image_buffer.o $(LODE)lodepng.o $(FLAGS)

clean:
	rm -f $(OUT)
	rm -f $(SRC)*.o
	rm -f $(LODE)*.o

block_color.o: $(SRC)block_color.cpp $(SRC)block_color.hpp
	$(CC) -std=c++0x -O3 -funroll-all-loops -c $(SRC)block_color.cpp -o $(SRC)block_color.o

image_buffer.o: $(SRC)image_buffer.cpp $(SRC)image_buffer.hpp
	$(CC) -std=c++0x -O3 -funroll-all-loops -c $(SRC)image_buffer.cpp -o $(SRC)image_buffer.o

lodepng.o: $(LODE)lodepng.cpp $(LODE)lodepng.hpp
	$(CC) -std=c++0x -O3 -funroll-all-loops -c $(LODE)lodepng.cpp -o $(LODE)lodepng.o
