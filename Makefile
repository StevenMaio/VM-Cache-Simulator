THREAD = -lpthread -lrt
CC = gcc
MEM_TARGET = mem
MEM_SRC = mem.c

mem: clean
	$(CC) $(MEM_SRC) $(THREAD) -o $(MEM_TARGET)

clean:
	rm -f mem
