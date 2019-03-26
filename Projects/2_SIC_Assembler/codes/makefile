CC = gcc
CFLAGS = -Wall -W -g
OBJECTS = 20171690.o shell.o memory.o opcode.o assembler.o
TARGET = 20171690.out

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -rf $(OBJECTS) $(TARGET) $(TARGET).dSYM
