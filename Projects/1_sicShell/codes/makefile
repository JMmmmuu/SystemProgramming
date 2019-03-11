CC = gcc
CFLAGS = -Wall -W -g
OBJECTS = main.o shell.o memory.o opcode.o
TARGET = 20171690.out

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -r $(OBJECTS)
