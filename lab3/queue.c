/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

// queue.c file
extern PROC *freeList;

int enqueue(PROC **queue, PROC *p)
{
  PROC *q  = *queue;
  int SR = int_off();
  if (q==0 || p->priority > q->priority){
    *queue = p;
    p->next = q;
    int_on(SR);
    return;
  }
  while (q->next && p->priority <= q->next->priority){
    q = q->next;
  }
  p->next = q->next;
  q->next = p;
  int_on(SR);
}

PROC *dequeue(PROC **queue)
{
  PROC *p = *queue;
  int SR = int_off();
  if (p)
    *queue = p->next;
  int_on(SR);
  return p;
}

int printList(char *name, PROC *p)
{
   kprintf("%s = ", name);
   while(p){
     kprintf("[%d%d]->", p->pid, p->priority);
     p = p->next;
  }
  kprintf("NULL\n"); 
}

int printSiblings(char *name, PROC *p)
{
   kprintf("%s = ", name);
   while(p){
     kprintf("[%d%d]->", p->pid, p->priority);
     p = p->sibling;
  }
  kprintf("NULL\n"); 
}

