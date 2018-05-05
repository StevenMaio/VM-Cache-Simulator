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
#include "utils.h"

// I can probably change this later
#define MAX_ARGS 10

int const MAX_THREADS = 4; 
int const MAX_BUFFER_SIZE = 255;
int num_threads = 0;
int in_fd[2], out_fd[2];	// file descriptors
int unalloc_mem_curs = 0;
void *alloc_arr;

process_node *head = NULL;

// Main thread
int main(void) 
{
	// Initialize all the settings
	char *buffer, *mem_buffer, *cursor, *args[MAX_ARGS];
	process_node *pcursor;
	int pid, addr, virt_addr, value, num_args, fifo_in, fifo_out, found;
	pthread_t tid;
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
		found = 0;

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
			// Do not allow the user to create more than 4 processes
			if (num_threads == 4)
			{
				printf("Cannot create more processes\n");
				continue;
			}

			if (!init_process(&head))
			{
				// Print out the ID of the thread that was just created
				num_threads++;
				pcursor = head;	

				while (pcursor->next)
					pcursor = pcursor->next;

				printf("Process created: %lu\n", (unsigned long) pcursor->pid);
			}
		}

		else if (!strcmp(buffer, "list"))
		{
			list(head);
		}

		else if (!strcmp(buffer, "kill"))
		{
			tid = (pthread_t) strtoul(args[1], NULL, 10);
			pcursor = head;

			// Restart the loop if there are no cursors
			if (!head)
				continue;

			do
			{
				if (pcursor->pid == tid)
				{
					mem_list(pcursor);
					break;
				}

				pcursor = pcursor->next;
			} while (pcursor);
		}

		else if (!strcmp(buffer, "mem"))
		{
			tid = (pthread_t) strtoul(args[1], NULL, 10);
			pcursor = head;

			// Restart the loop if there are no cursors
			if (!head)
				continue;

			do
			{
				if (pcursor->pid == tid)
				{
					mem_list(pcursor);
					break;
				}

				pcursor = pcursor->next;
			} while (pcursor);
		}

		else if (!strcmp(buffer, "allocate"))
		{
			tid = (pthread_t) strtoul(args[1], NULL, 10);
			pcursor = head;

			// Restart the loop if there are no cursors
			if (head == NULL)
				continue;

			do
			{
				if (pcursor->pid == tid)
				{
					cse320_malloc(pcursor, &unalloc_mem_curs);
					break;
				}

				pcursor = pcursor->next;
			} while (pcursor);

			// TODO: Check to see if an error occured
		}

		else if (!strcmp(buffer, "read"))
		{
			tid = (pthread_t) strtoul(args[1], NULL, 10);
			virt_addr = atoi(args[2]);
			pcursor = head;

			// Check to see if the address is aligned
			if (virt_addr%4)
			{
				printf("Error : requested address out of alignment\n");
				continue;
			}

			// Restart the loop if there are no cursors
			if (head == NULL)
				continue;

			do
			{
				if (pcursor->pid == tid)
				{
					addr = cse320_virt_to_phys(pcursor, virt_addr);
					found = 1;
					break;
				}

				pcursor = pcursor->next;
			} while (pcursor);

			// Restart the loop if there are no cursors
			if (!found)
				continue;

			if (addr == -1)
			{
				printf("Error : requested address hasn't been allocated\n");
				continue;
			}

			// Output to memory, and then read from memory
			dprintf(fifo_out, "read %d\n%c", addr, 0);
			read(fifo_in, mem_buffer, MAX_BUFFER_SIZE);

			// TODO: Format the stuff
			printf("%s\n", mem_buffer);
		}

		// TODO: Implement this correctly
		else if (!strcmp(buffer, "write"))
		{
			tid = (pthread_t) strtoul(args[1], NULL, 10);
			virt_addr = atoi(args[2]);
			value = atoi(args[3]);
			pcursor = head;

			// Check to see if the address is aligned 
			if (virt_addr%4)
			{
				printf("Error : requested address out of alignment\n");
				continue;
			}

			// Restart the loop if there are no cursors
			if (head == NULL)
				continue;

			do
			{
				if (pcursor->pid == tid)
				{
					addr = cse320_virt_to_phys(pcursor, virt_addr);
					found = 1;
					break;
				}

				pcursor = pcursor->next;
			} while (pcursor);

			// Restart the loop if there are no cursors
			if (!found)
				continue;

			if (addr == -1)
			{
				printf("Error : requested address hasn't been allocated\n");
				continue;
			}

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
