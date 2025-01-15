CC = gcc
CC_WIN = x86_64-w64-mingw32-gcc
OBJ_DIR = ./object_files/

C_FLAGS = -Wall -Wextra -Wconversion -std=c99 -fdiagnostics-color=always -pedantic
DEBUG_FLAG = -g

LIB_DIR = -L ./lib

LINKERS = $(LIB_DIR) -lraylib /usr/lib/x86_64-linux-gnu/libGL.so -lm -lpthread -ldl -lrt
LINKERS_WIN = $(LIB_DIR) -lraylibwin -lopengl32 -lgdi32 -lwinmm 
#-lraylibdll

C_FILES = $(wildcard *.c)
OBJ_FILES_LIN = $(patsubst %.c, $(OBJ_DIR)%_lin.o, $(C_FILES))
OBJ_FILES_WIN = $(patsubst %.c, $(OBJ_DIR)%_win.o, $(C_FILES))

build: main

main: $(OBJ_FILES_LIN)
	$(CC) $(DEBUG_FLAG) $(C_FLAGS) -o main $(OBJ_DIR)*_lin.o $(LINKERS)

$(OBJ_FILES_LIN): $(OBJ_DIR)%_lin.o: %.c
	$(CC) -c $(DEBUG_FLAG) $< -o $@

win: main.exe

main.exe: $(OBJ_FILES_WIN)
	$(CC_WIN) $(C_FLAGS) -static -o main $(OBJ_DIR)*_win.o $(LINKERS_WIN)

$(OBJ_FILES_WIN): $(OBJ_DIR)%_win.o: %.c
	$(CC_WIN) -c $(DEBUG_FLAG) $< -o $@

clean: 
	rm -f $(OBJ_DIR)*.o main.exe main

run: 
	./main
