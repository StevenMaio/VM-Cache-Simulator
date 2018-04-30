#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS 10

int const MAX_SIZE = 1024;
int const MAX_BUFFER_SIZE = 255;
void *memory;

int main(void)
{
	// main loop ting
	char *buffer, *cursor, *args[MAX_ARGS];
	int addr, value, loop, num_args, i, fifo_in, fifo_out;

	// Initialize main memory (1024 bytes of mem)
	memory = calloc(1024, 1);
	buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
	loop = 1;

	fifo_in = open("fifo_in", O_RDONLY);
	fifo_out = open("fifo_out", O_WRONLY);

	sleep(1);

	// Main loop of the program
	while (loop)
	{
		// Read a line from input
		*buffer = 0;
//		fflush(fifo_fd);
//		fgets(buffer, MAX_BUFFER_SIZE, stdin);
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
				dprintf(fifo_out, "Error : requested address out of alignment\n");
				continue;
			}

			else if (addr < 0 || addr >= 1024)
			{
				dprintf(fifo_out, "Error : address out of range\n");
				continue;
			}

			// Print hte value
			value = *(int*)(memory + addr);
			dprintf(fifo_out, "%d\n", value);
		}

		else if (!strcmp(buffer, "write"))
		{
			if (num_args != 3)
			{
				dprintf(fifo_out, "Error : incorrect number of arguments\n");
				continue;
			}

			addr = atoi(args[1]);
			value = atoi(args[2]);

			if (addr%4)
			{
				dprintf(fifo_out, "Error : requested address out of alignment\n");
				continue;
			}

			else if (addr < 0 || addr >= 1024)
			{
				dprintf(fifo_out, "Error : address out of range\n");
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
