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
cache *cache_set;

process_node *head = NULL;

// Main thread
int main(void) 
{
	// Initialize all the settings
	char *buffer, *mem_buffer, *cursor, *args[MAX_ARGS];
	process_node *pcursor;
	int pid, addr, prev_addr, virt_addr, value, num_args, 
		fifo_in, fifo_out, found, modified, prev_value, id_gen;
	pid_t child_pid;

	buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
	mem_buffer = (char*) malloc(sizeof(char) * MAX_BUFFER_SIZE);
	cache_set = init_cache();
	id_gen = 0;

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

			if (!init_process(&head, id_gen))
			{
				// Print out the ID of the thread that was just created
				num_threads++;
				id_gen++;
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

		else if (!strcmp(buffer, "mem"))
		{
			pid = atoi(args[1]);
			pcursor = head;

			if (num_args != 2)
			{
				printf("Error : incorrect number of arguments\n");
				continue;
			}

			// Restart the loop if there are no cursors
			if (!head)
				continue;

			do
			{
				if (pcursor->pid == pid)
				{
					mem_list(pcursor);
					break;
				}

				pcursor = pcursor->next;
			} while (pcursor);
		}

		else if (!strcmp(buffer, "allocate"))
		{
			pid = atoi(args[1]);
			pcursor = head;

			if (num_args != 2)
			{
				printf("Error : incorrect number of arguments\n");
				continue;
			}

			// Restart the loop if there are no cursors
			if (head == NULL)
				continue;

			// Call alloc in mem c
			dprintf(fifo_out, "alloc\n%c", 0);
			read(fifo_in, mem_buffer, MAX_BUFFER_SIZE);
			addr = atoi(mem_buffer);

			do
			{
				if (pcursor->pid == pid)
				{
					cse320_malloc_helper(pcursor, addr);
					break;
				}

				pcursor = pcursor->next;
			} while (pcursor);

			// TODO: Check to see if an error occured
		}

		else if (!strcmp(buffer, "kill"))
		{
			pid = atoi(args[1]);
			pcursor = head;

			if (num_args != 2)
			{
				printf("Error : incorrect number of arguments\n");
				continue;
			}

			// Restart the loop if there are no cursors
			if (head == NULL)
				continue;

			if (!kill_process(&head, pid))
			{
				num_threads--;
			}

			// TODO: Handle if an error occured
		}

		else if (!strcmp(buffer, "read"))
		{
			pid = atoi(args[1]);
			virt_addr = atoi(args[2]);
			pcursor = head;

			if (num_args != 3)
			{
				printf("Error : incorrect number of arguments\n");
				continue;
			}

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
				if (pcursor->pid == pid)
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

			// TODO: Check to see if the addr is cached
			if (is_cached(cache_set, addr, &prev_addr, &prev_value, &modified))
			{
				printf("cache hit\n");
				printf("%d\n", prev_value);
				continue;
			} 

			// Access the address from 'main mempry'
			printf("cache miss\n");
			dprintf(fifo_out, "read %d\n%c", addr, 0);
			read(fifo_in, mem_buffer, MAX_BUFFER_SIZE);
			sleep(1);
			value = atoi(mem_buffer);

			// If the cache wasn't valid, add this to the cache
			if (prev_addr == -1)
			{
				// When the cachine line isn't valid
				set_cache(cache_set, addr, value, 0);
			}

			else
			{
				// Send the new value to main memory if the cache was modified
				if (modified)	
				{
					// Write the new value into memory
					dprintf(fifo_out, "write %d %d\n%c", prev_addr, prev_value, 0);
					sleep(1);
				}

				set_cache(cache_set, addr, value, 0);
			}

			printf("%d\n", value);
		}

		// TODO: Implement this correctly
		else if (!strcmp(buffer, "write"))
		{
			pid = atoi(args[1]);
			virt_addr = atoi(args[2]);
			value = atoi(args[3]);
			pcursor = head;

			if (num_args != 4)
			{
				printf("Error : incorrect number of arguments\n");
				continue;
			}

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
				if (pcursor->pid == pid)
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

			// Check to see if the addr is cached
			if (is_cached(cache_set, addr, &prev_addr, &prev_value, &modified))
			{
				// Write the value to the cache
				printf("cache hit\n");
				set_cache(cache_set, addr, value, 1);
				continue;
			} 

			printf("cache miss\n");
			sleep(1);

			if (addr == -1)
			{
				printf("Error : requested address hasn't been allocated\n");
				continue;
			}

			// Otherwise, read the data from main memory, and then cache it
			if (prev_addr >= 0)
			{
				// If the cache has been modified, write the new value into
				// main memory
				if (modified)	
				{
					dprintf(fifo_out, "write %d %d\n%c", prev_addr, prev_value, 0);
				}
			}

			set_cache(cache_set, addr, value, 1);
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
