#include <stdlib.h>
#include "structs.h"

/*
 * This function frees the page table and all of its second layer tables
 */
int free_page_table(process_node *process) {
	page_table *pt = process->pt;
	sec_lvl_pte *sec_lvl;

	int i, j;
	for (i = 0; i < PTE_ENTRIES; i++)
	{
		sec_lvl = pt->entries[i];
		free(sec_lvl);
	}

	free(pt);
}

/* 
 * Initialize a page table entry with all of the address set to null
 */
page_table *init_page_table()
{
	// Initialize a page table entry
	page_table *pt = (page_table*) malloc(sizeof(page_table));
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

		pt->entries[i] = sub_pte;
	}

	return pt;
}
