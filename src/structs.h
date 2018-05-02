#ifndef STRUCTS_H
#define STRUCTS_H

#include <pthread.h>
#define PTE_ENTRIES 8

typedef struct sec_lvl_pt_entry {
	void *addr[PTE_ENTRIES];
} sec_lvl_pte;

typedef struct first_lvl_pt_entry {
	sec_lvl_pte *entries[PTE_ENTRIES];
} pt_entry ;

typedef struct procss_node {
	pthread_t pid;
	pt_entry *page_table;
	struct procss_node *next;
} process_node;

#endif
