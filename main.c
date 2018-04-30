#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// I can probably change this later
#define MAX_ARGS 10

int const MAX_THREADS = 4; 
int const MAX_BUFFER_SIZE = 255;
int num_threads = 0;

// Main thread
int main(void) 
{
	// Initialize all the settings
	char *buffer, *cursor, *args[MAX_ARGS];
	int pid, addr, value, i;
	int fd[2];	// file descriptors

	buffer = NULL:
}
