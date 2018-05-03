#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

/*
 * Thread protocol. At the moment, this doesn't do anything interesting.
 */
void *thread_func(void* vargp)
{
	// Loop infinitely
	while(1);
}

/*
 * This function frees the page table and all of its second layer tables
 */
int free_page_table(process_node *process) {
	pt_entry page_table = process->page_table;
	sec_lvl_pte sec_lvl;

	int i, j;
	for (i = 0; i < PTE_ENTRIES; i++)
	{
		sec_lvl = page_table.entries[i];
		free(sec_lvl);
	}

	free(page_table);
}

/* 
 * Initialize a page table entry with all of the address set to null
 */
pt_entry *init_page_table()
{
	// Initialize a page table entry
	pt_entry *pte = (pt_entry*) malloc(sizeof(pt_entry));
	sec_lvl_pte *sub_pte;
	int i, j;
	
	for (i = 0; i < PTE_ENTRIES; i++)
	{
		// Initialize second level page table
		sub_pte = (sec_lvl_pte*) malloc(sizeof(sec_lvl_pte));
		for (j = 0; j < PTE_ENTRIES; j++)
		{
			sub_pte->addr[j] = NULL;
		}

		pte->entries[i] = sub_pte;
	}

	return pte;
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
	node->page_table = init_page_table();
	node->pid = pid;
	node->next = NULL;

	return node;
}
