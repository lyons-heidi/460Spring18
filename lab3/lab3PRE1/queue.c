#include <stdio.h>
#include "type.h"

int enqueue(PROC **queue, PROC *p)
{
    // enter p into queue by priority
    PROC *current = *queue;

    // If list is empty, add by default
    if (*queue == NULL){
        p->next = 0;
        *queue = p;
        return 1;
    }

    else{
        // If the PROC inserted has the highest priority, the head needs to be swapped
        if (p->priority > current->priority){
            *queue = p;
            p->next = current;
        }
        else{
            // loop to the point where the next one is less than the current priority node
            while(current->next && current->next->priority >= p->priority){
                current = current->next;
            }
            
            // Place p (node to be added) in place of currrent
            p->next = current->next;
            current->next = p;
        }
    }
    return 0;
}

PROC *dequeue(PROC **queue)
{
    // remove & return first PROC from queue
    if ((*queue) == NULL){
        return NULL;
    }

    PROC *first = *queue;
    *queue = (*queue)->next;
    return first;
}


int printList(char *name, PROC *p)
{
   printf("%s = ", name);
   // print list elements as [pid priority] -> ....
   while(p){
        printf("[%d %d] -> ", p->pid, p->priority);
        p = p->next;
    }
    printf("NULL \n");
    return 1;
}