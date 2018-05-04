#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

/*
 * Lists the process ids of all active processes in the linked list
 */
void list(process_node *head)
{
	if (head == NULL)
		return;

	printf("Thread id: %d\n", (int) head->pid);
	list(head->next);
}

/*
 * Thread protocol. At the moment, this doesn't do anything interesting.
 */
void *thread_func(void* vargp)
{
	// Loop infinitely
	while(1);
}

process_node *init_process(process_node *head)
{
	// create a new process
	int i;
	pthread_t pid;
	process_node *node;

	// TODO: Create a new thread
	if (pthread_create(&pid, NULL, thread_func, NULL)) {
		// IF an error occurred
		printf("An error occurred creating a new process\n");
		return NULL;
	}
	
	node = (process_node*) malloc(sizeof(process_node));
	node->pt= init_page_table();
	node->pid = pid;
	node->next = NULL;

	return node;
}
