CC=gcc
CFLAGS=-std=c11 -Wall -O3 -D_GNU_SOURCE
LDFLAGS=
LIBS=-lSDL2
OBJ=$(patsubst %.c, %.o, $(wildcard *.c))
TARGET=8mu
.PHONY: all clean
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $(TARGET) $(LIBS)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJ) $(TARGET)
