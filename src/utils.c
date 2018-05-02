#include <pthread.h>
#include <stdlib.h>
#include "structs.h"

void thread_func(void)
{
	// Loop infinitely
	while(1);
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
	process_node *node = (process_node*) malloc(sizeof(process_node));
	node->page_table = init_page_table();

	node->page_table = init_page_table();

	// create a a new thread
	return node;
}
