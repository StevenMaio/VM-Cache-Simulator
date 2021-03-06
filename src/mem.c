#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_ARGS 10
#define MAX_ALLOC 256

int const MAX_SIZE = 1024;
int const MAX_BUFFER_SIZE = 255;
int allocated[MAX_ALLOC];
int cursor = 0;
void *memory;

void deallocate(int pid)
{
	int i;
	pid = pid << 1;

	for (i = 0; i < MAX_ALLOC; i++)
	{
		if (pid == (allocated[i] & -2))
		{
			allocated[i] = 0;
		}
	}
}

/*
 * This function will allocate memory. If it returns a positive integer, then
 * this integer represents a valid address.
 *
 * If this returns -1, then there does not exist any free space.
 *
 */
int allocate(int pid)
{
	int temp, position;
	position = cursor;

	// Look for unallocated space 
	while (allocated[position/4] & 1)
	{
		position = (position + 4) % MAX_SIZE;

		if (position == cursor)
		{
			return -1;
		}
	}

	// Set the bit to being allocated
	allocated[position/4] = 1 + (pid << 1);
	temp = position;

	cursor = (position + 4) % MAX_SIZE;
	return temp;
}

int main(void)
{
	// main loop ting
	char *buffer, *cursor, *args[MAX_ARGS];
	int addr, value, loop, num_args, i, fifo_in, fifo_out, pid;
	sigset_t mask;

	// Determine if an error occurred while setting hte mask
	if (sigemptyset(&mask) || sigaddset(&mask, SIGINT))
	{
		printf("An error occurred\n");
		exit(0);
	}

	sigprocmask(SIG_BLOCK, &mask, NULL);

	// Initialize main memory (1024 bytes of mem)
	memory = calloc(1024, 1);
	buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
	loop = 1;

	// Set the allocaton flag to 0 for everything
	for (i = 0; i < MAX_ALLOC; i++)
	{
		allocated[i] = 0;
	}

	sleep(5);

	fifo_in = open("fifo_1", O_RDONLY);
	fifo_out = open("fifo_2", O_WRONLY);

	// Main loop of the program
	while (loop)
	{
		// Read a line from input
		*buffer = 0;
		fsync(fifo_out);
		read(fifo_in, buffer, MAX_BUFFER_SIZE);

		// Process the input
		strtok(buffer, "\n");
		strtok(buffer, " ");
		args[0] = buffer;
		cursor = buffer;
		num_args = 1;

		while(cursor = strtok(NULL, " "))
		{
			args[num_args] = cursor;
			num_args++;
		}

		if (!strcmp(buffer, "read"))
		{
			// check to see if x is properly alg
			addr = atoi(args[1]);

			if (addr%4)
			{
				dprintf(STDOUT_FILENO, "Error : requested address out of alignment\n");
				continue;
			}

			else if (addr < 0 || addr >= 1024)
			{
				dprintf(STDOUT_FILENO, "Error : address out of range\n");
				continue;
			}

			// Print hte value
			value = *(int*)(memory + addr);
			dprintf(fifo_out, "%d%c", value, 0);
		}

		else if (!strcmp(buffer, "alloc"))
		{
			pid = atoi(args[1]);

			// Allocate new memory and return it
			addr = allocate(pid);
			dprintf(fifo_out, "%d%c", addr, 0);
		}

		else if (!strcmp(buffer, "dealloc"))
		{
			pid = atoi(args[1]);

			// Allocate new memory and return it
			deallocate(pid);
			dprintf(STDOUT_FILENO, "Memory freed\n%c", 0);
		}

		else if (!strcmp(buffer, "write"))
		{
			if (num_args != 3)
			{
				dprintf(STDOUT_FILENO, "Error : incorrect number of arguments\n");
				continue;
			}

			addr = atoi(args[1]);
			value = atoi(args[2]);

			if (addr%4)
			{
				dprintf(STDOUT_FILENO, "Error : requested address out of alignment\n");
				continue;
			}

			else if (addr < 0 || addr >= 1024)
			{
				dprintf(STDOUT_FILENO, "Error : address out of range\n");
				continue;
			}

			*((int*)(memory + addr)) = value;
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

	close(fifo_in);
	close(fifo_out);
	free(memory);
	free(buffer);
	return 0;
}
