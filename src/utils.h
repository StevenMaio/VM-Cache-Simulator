#ifndef UTILS_H
#define UTILS_H

#include "structs.h"
#include <pthread.h>

int init_process(process_node **head, int pid);

int kill_process(process_node **head, pthread_t tid, int *pid);

void list(process_node *head);

void mem_list(process_node *process_node);

int malloc_helper(process_node *node, int addr);

int virt_to_phys(process_node *node, int address);

int find_node(process_node *head, pthread_t tid, process_node **cursor);

#endif
