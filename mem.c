#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 10

int const MAX_SIZE = 1024;
int const MAX_BUFFER_SIZE = 255;
void *memory;

int main(void)
{
	// main loop ting
	char *buffer, *cursor, *args[MAX_ARGS];
	int addr, value, loop, num_args, i;

	// Initialize main memory (1024 bytes of mem)
	memory = calloc(1024, 1);
	buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
	loop = 1;

	// Main loop of the program
	while (loop)
	{
		// Read a line from input
		*buffer = 0;
		fflush(stdout);
		printf("> ");
		fgets(buffer, MAX_BUFFER_SIZE, stdin);

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

		// TODO: This part can be removed later
		for (i = 0; i < num_args; i++)
			printf("%s\n", args[i]);
	}

	free(memory);
	free(buffer);
	return 0;
}
