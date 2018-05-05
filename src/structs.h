#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>
#define PT_ENTRIES 8
#define CACHE_SIZE 4

typedef struct sec_lvl_pt_entry {
	int addr[PT_ENTRIES];
} page_entry;

typedef struct first_lvl_pt_entry {
	page_entry *entries[PT_ENTRIES];
} page_table;

typedef struct procss_node {
	pthread_t pid;
	page_table *pt;
	struct procss_node *next;
} process_node;

typedef struct cash {
	int valid;
	int tag;
	int value;
} cache;

/*
 * This function frees the page table and all of its second layer tables
 */
void free_page_table(process_node *process_node);

/* 
 * Initialize a page table entry with all of the address set to null
 */
page_table *init_page_table();

page_entry *init_page_entry();

int get_tag(int addr);

/*
 * Initializes a cache set
 */
cache *init_cache();

int is_cached(*cache cache_set, int addr, int *value);

int set_cache(*cache cache_set, int addr, int value);

#endif
