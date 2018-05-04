#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>
#define PTE_ENTRIES 8

typedef struct sec_lvl_pt_entry {
	void *addr[PTE_ENTRIES];
} sec_lvl_pte;

typedef struct first_lvl_pt_entry {
	sec_lvl_pte *entries[PTE_ENTRIES];
} page_table;

typedef struct procss_node {
	pthread_t pid;
	page_table *pt;
	struct procss_node *next;
} process_node;

/*
 * This function frees the page table and all of its second layer tables
 */
int free_page_table(process_node *process_node);

/* 
 * Initialize a page table entry with all of the address set to null
 */
page_table *init_page_table();

#endif
