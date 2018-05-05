#include <stdlib.h>
#include "structs.h"

/*
 * This function frees the page table and all of its second layer tables
 */
void free_page_table(process_node *process) {
	page_table *pt = process->pt;
	page_entry *sec_lvl;

	int i, j;
	for (i = 0; i < PT_ENTRIES; i++)
	{
		sec_lvl = pt->entries[i];

		// Free sec_lvl if it has been allocated
		if (sec_lvl)
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
	page_entry *sub_pte;
	int i, j;
	
	for (i = 0; i < PT_ENTRIES; i++)
	{
		pt->entries[i] = NULL;
	}

	return pt;
}

page_entry *init_page_entry() 
{
	page_entry *pe = (page_entry*) malloc(sizeof(page_entry));
	int i;

	for (i = 0; i < PT_ENTRIES; i++)
	{
		pe->addr[i] = -1;
	}

	return pe;
}

int get_tag(int addr)
{
	int upper, lower;

	lower = addr & 0xF;
	upper = (addr >> 2) & 0xF0;

	return lower + upper;
}

cache *init_cache()
{
	int i;
	cache *cache_set = (cache*) malloc(sizeof(cache) * CACHE_SIZE);
	cache cursor;

	// set the valid bit of each one thing to 0
	for (i = 0; i < CACHE_SIZE; i++)
	{
		(cache_set + i)->valid = 0;
	}

	return cache_set;
}
