#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define READ 0
#define WRITE 1

// I can probably change this later
#define MAX_ARGS 10

int const MAX_THREADS = 4; 
int const MAX_BUFFER_SIZE = 255;
int num_threads = 0;
int in_fd[2], out_fd[2];	// file descriptors

// Main thread
int main(void) 
{
	// Initialize all the settings
	char *buffer, *mem_buffer, *cursor, *args[MAX_ARGS];
	int pid, addr, value, num_args, success; 
	pid_t child_pid;

	buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
	mem_buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);

	/* Create a child process which will run the memory 
	 * This process have it's stdout and stdin redirected using pipes
	 */
	if (pipe(in_fd) < 0)
	{
		printf("Error\n");
		return 0;
	}

	if (pipe(out_fd) < 0)
	{
		printf("Pipe error\n");
		return 0;
	}

	if ((child_pid = fork()) == 0)
	{
		close(out_fd[WRITE]);
		close(in_fd[READ]);

		if (dup2(out_fd[READ], STDIN_FILENO) < 0)
		{
			
			fprintf(stderr, "Error overwriting stdin\nErrno: %d\n", errno);
			return 0;
		}

		// Dupe over stdout
		fflush(stdout);

		if (dup2(in_fd[WRITE], STDOUT_FILENO) < 0)
		{
			fprintf(stderr, "Error overwriting stdout\nErrno: %d\n", errno);
			return 0;
		}

		char *argv[] = {"./mem", NULL};
		execvp("./mem", argv);
		fputs("Error creating new process\n", stderr);
		exit(-1);
	}

	close(out_fd[READ]);
	close(in_fd[WRITE]);
	sleep(1);

	while (1)
	{
		*buffer = 0;
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


		// TODO: Implement this correctly in the future
		if (!strcmp(buffer, "read"))
		{
			addr = atoi(args[1]);

			// TODO: Check for errors

			// Output to memory, and then read from memory
			dprintf(out_fd[WRITE], "read %d\n", addr);
			read(in_fd[READ], mem_buffer, MAX_BUFFER_SIZE);

			// TODO: Format the stuff
			printf("%s", mem_buffer);
		}

		// TODO: Implement this correctly
		else if (!strcmp(buffer, "write"))
		{
			addr = atoi(args[1]);
			value = atoi(args[2]);

			dprintf(out_fd[WRITE], "write %d %d\n", addr, value);
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
	dprintf(out_fd[WRITE], "exit\n");

	if (buffer)
		free(buffer);

	return 0;
}
