#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include "structs.h"

// I can probably change this later
#define MAX_ARGS 10

int const MAX_THREADS = 4; 
int const MAX_BUFFER_SIZE = 255;
int num_threads = 0;
int in_fd[2], out_fd[2];	// file descriptors
int unalloc_mem_curs = 0;
void *alloc_arr;

struct thread_node *head = NULL;

// Main thread
int main(void) 
{
	// Initialize all the settings
	char *buffer, *mem_buffer, *cursor, *args[MAX_ARGS];
	int pid, addr, value, num_args, fifo_in, fifo_out;
	pid_t child_pid;

	buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
	mem_buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
	alloc_arr = malloc(1024/4);

	// Open up the pipelin
	fifo_out = open("fifo_1", O_WRONLY);
	fifo_in = open("fifo_2", O_RDONLY);

	sleep(1);

	while (1)
	{
		*buffer = 0;
		fsync(fifo_out);
		fflush(stdout);
		printf("> ");
		fgets(buffer, MAX_BUFFER_SIZE, stdin);

		strtok(buffer, "\n");
		strtok(buffer, " ");
		args[0] = buffer;
		cursor = buffer;
		num_args = 1;

		// Check to see if cursor is pointing to a null character
		if (*cursor == 0)
			continue;

		while (cursor = strtok(NULL, " "))
		{
			args[num_args] = cursor;
			num_args++;
		}

		if (!strcmp(buffer, "create"))
		{

		}

		// TODO: Implement this correctly in the future
		if (!strcmp(buffer, "read"))
		{
			addr = atoi(args[1]);

			// TODO: Check for errors

			// Output to memory, and then read from memory
			dprintf(fifo_out, "read %d\n%c", addr, 0);
			read(fifo_in, mem_buffer, MAX_BUFFER_SIZE);

			// TODO: Format the stuff
			printf("%s\n", mem_buffer);
		}

		// TODO: Implement this correctly
		else if (!strcmp(buffer, "write"))
		{
			addr = atoi(args[1]);
			value = atoi(args[2]);

			dprintf(fifo_out, "write %d %d\n%c", addr, value, 0);
		}

		else if (!strcmp(buffer, "exit"))
		{
			break;
		}

		else 
		{
			printf("Unknown command\n");
		}
	}

	// End the mem process
	dprintf(fifo_out, "exit\n%c", 0);

	close(fifo_in);
	close(fifo_out);
	if (buffer)
		free(buffer);

	return 0;
}
