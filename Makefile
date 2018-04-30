THREAD = -lpthread -lrt
CC = gcc -g
MAIN_TARGET = main
MAIN_SRC = main.c
MEM_TARGET = mem
MEM_SRC = mem.c
FIFO_SRC = create_fifo.c
FIFO_TARGET = fifo

all: clean mem fifo_in fifo_out
	$(CC) $(MAIN_SRC) $(THREAD) -o $(MAIN_TARGET)

mem: clean
	$(CC) $(MEM_SRC) $(THREAD) -o $(MEM_TARGET)

fifo_in:
	mkfifo -m a=rw fifo_in

fifo_out:
	mkfifo -m a=rw fifo_out

clean:
	rm -f mem main fifo*
