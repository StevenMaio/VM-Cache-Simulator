#ifndef UTILS_H
#define UTILS_H

#include "structs.h"
#include <pthread.h>

int init_process(process_node **head, int pid);

int kill_process(process_node **head, pthread_t tid);

void list(process_node *head);

void mem_list(process_node *process_node);

int cse320_malloc_helper(process_node *node, int addr);

int cse320_virt_to_phys(process_node *node, int address);

#endif
