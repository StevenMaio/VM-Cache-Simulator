THREAD = -lpthread -lrt
CC = gcc -g
MAIN_TARGET = main
MAIN_SRC = main.c
MEM_TARGET = mem
MEM_SRC = mem.c

main: clean mem
	$(CC) $(MAIN_SRC) $(THREAD) -o $(MAIN_TARGET)

mem: clean
	$(CC) $(MEM_SRC) $(THREAD) -o $(MEM_TARGET)

clean:
	rm -f mem main
