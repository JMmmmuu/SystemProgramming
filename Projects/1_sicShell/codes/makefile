CC = gcc
CFLAGS = -Wall -W -g
OBJECTS = main.o shell.o memory.c opcode.c
TARGET = 20171690.out

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -r $(OBJECTS)
