THREAD = -pthread
B = -lrt
CC = gcc -g
MAIN_TARGET = main
MAIN_SRC = src/main.c
UTIL_SRC = src/utils.c src/structs.c
MEM_TARGET = mem
MEM_SRC = src/mem.c
FIFO_SRC = create_fifo.c
FIFO_TARGET = fifo

all: main

main: clean mem fifo_in fifo_out
	$(CC) $(UTIL_SRC) $(MAIN_SRC) $(THREAD) -o $(MAIN_TARGET)

mem: clean
	$(CC) $(MEM_SRC) $(THREAD) -o $(MEM_TARGET)

fifo_in:
	mkfifo -m a=rw fifo_1

fifo_out:
	mkfifo -m a=rw fifo_2

clean:
	rm -f mem main fifo*
