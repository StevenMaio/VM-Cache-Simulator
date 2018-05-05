#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

// Array for keeping track of allocated and unallocated addresses
//void *mem_alloc_arr = calloc(256, 1);

/*
 * Thread protocol. At the moment, this doesn't do anything interesting.
 */
void *thread_func(void* vargp)
{
	// Loop infinitely
	while(1);
}

/*
 * Lists the process ids of all active processes in the linked list
 */
void list(process_node *head)
{
	if (head == NULL)
		return;

	printf("Process ID: %lu\n", (unsigned long) head->pid);
	list(head->next);
}

void mem_list(process_node *process)
{
	int i, j, virt_addr, address;
	pthread_t pid;
	page_table pt;
	page_entry *cursor;

	pid = process->pid;
	pt = *(process->pt);

	for (i = 0; i < PT_ENTRIES; i++)
	{
		cursor = pt.entries[i];

		// If cursor is null, skip over it
		if (cursor)
		{	
			for (j = 0; j < PT_ENTRIES; j++)
			{
				virt_addr = cursor->addr[j];

				if (virt_addr > -1)
				{
					// Construct the value of the pointer 
					address = i << 10;
					address += j;
					address = address << 12;

					printf("%d\n", address);
				}
			}
		}
	}
}

int init_process(process_node **head)
{
	// create a new process
	int i;
	pthread_t pid;
	process_node *node, *cursor;

	// TODO: Create a new thread
	if (pthread_create(&pid, NULL, thread_func, NULL)) {
		// IF an error occurred
		printf("An error occurred creating a new process\n");
		return 1;
	}
	
	node = (process_node*) malloc(sizeof(process_node));
	node->pt = init_page_table();
	node->pid = pid;
	node->next = NULL;

	// If head is null, return the new node.
	if (*head == NULL) {
		// Set the value of *head to node
		*head = node;

		return 0;
	}

	cursor = *head;

	while (cursor->next)
		cursor = cursor->next;

	cursor->next = node;
	return 0;
}

/*
 * Helper method which creates a virtual address
 */
int get_virt_addr(int pt_entry_no, int entry_no)
{
	int virt_addr = pt_entry_no << 10;
	virt_addr += entry_no;
	virt_addr = virt_addr << 10;

	return virt_addr;
}

int cse320_malloc(process_node *node, int *unalloc_cursor) {
	page_table *pt = node->pt;
	page_entry *cursor;
	int i, j, found, virt_addr;
	
	j = 0, found = 0;

	// Iterate over the page table until we find an unusred location in memory
	for (i = 0; i < PT_ENTRIES; i++)
	{
		cursor = pt->entries[i];

		// Handle allocating a second level entry
		if (! cursor)
		{
			// Allocate a new page_entry
			pt->entries[i] = init_page_entry();
			cursor = pt->entries[i];

			cursor->addr[0] = *unalloc_cursor;
			*(unalloc_cursor) = (*unalloc_cursor  + 4)%1024;
			found = 1;
			break;
		}

		// Otherwise, attempt to find an unused entry
		for (j = 0; j < PT_ENTRIES; j++)
		{
			// If we found unused space, allocate it
			if (cursor->addr[j] == -1)
			{
				cursor->addr[j] = *unalloc_cursor;
				*(unalloc_cursor) = (*unalloc_cursor  + 4)%1024;
				found = 1;
				break;
			}
		}

		// If an address was found, exit the loop
		if (found)
		{
			return virt_addr = get_virt_addr(i, j);
		}
	}

	// Return the new virtual address, or -1 if no room was found
	if (found)
		return virt_addr;

	return -1;
}

/*
 * Returns the corresponding 'physical' address. If the physical address
 * has not been allocated yet, the this function returns a null pointer
 */
int cse320_virt_to_phys(process_node *node, int address) {
	page_table pt;
	page_entry *pte;
	int pte_no, entry_no;
	unsigned int phys_add;

	pte_no = address >> 22;
	entry_no = address >> 12 & 0x3FF;

	pt = *(node->pt);
	pte = pt.entries[pte_no];

	if (pte)
	{
		phys_add = pte->addr[entry_no];
		return phys_add;
	} else
	{
		return -1;
	}
}
