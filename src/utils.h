#ifndef UTILS_H
#define UTILS_H

#include "structs.h"
#include <pthread.h>

int init_process(process_node **head);

int kill_process(process_node **head, pthread_t tid);

void list(process_node *head);

void mem_list(process_node *process_node);

int init_process(process_node **head);

int cse320_malloc(process_node *node, int *unalloc_cursor);

int cse320_virt_to_phys(process_node *node, int address);

#endif
