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

int addr_from_cache(int set_no, int tag)
{
	int upper, lower;

	lower = tag & 0xF;
	upper = (tag & 0xF0) + (set_no << 2);
	upper = upper << 2;

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

/*
 * Checks to see if the address is cached inside the cache set.
 * If it is, then the value is placed into address of value, if value
 * is not null.
 *
 * Returns 1 if the addr is cached and sets value to the value in the cache.
 * If the addr is cached, then if the line is valid, the prev_addr is set to 
 * the address stored in cache, and value is set to the value. If valid is 
 * not set, then prev_addr is set to -1.
 */
int is_cached(cache *cache_set, int addr, int *prev_addr, int *value,
		int *modified)
{
	if (!cache_set)
	{
		return 0;
	}

	cache line;
	int set_no, tag;
	
	// get the offset and tag of the address
	set_no = (addr >> 4) & 3;
	tag = get_tag(addr);

	line = *(cache_set + set_no);

	if (line.valid & 1)
	{
		if (line.valid & 2)
		{
			*modified = 1;
		}

		if (line.tag == tag)
		{
			*value = line.value;
			return 1;
		}

		*value = line.value;
		*prev_addr = addr_from_cache(set_no, line.tag);
		return 0;
	}

	*prev_addr = -1;

	return 0;
}

/*
 * Stores the correct address and value in the cache. Set mod_flag to 1, if
 * the value is already cached.
 *
 * Returns a 0 on success, and a 1 on failure.
 */
int set_cache(cache *cache_set, int addr, int value, int mod_flag)
{
	if (!cache_set)
	{
		return 1;
	}

	cache *line;
	int set_no, tag;

	set_no = (addr >> 4) & 3;
	tag = get_tag(addr);

	line = cache_set + set_no;

	line->valid = 1 + mod_flag*2;
	line->tag = tag;
	line->value = value;

	return 0;
}
