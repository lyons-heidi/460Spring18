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

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf
 
typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;

  int    priority;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    kstack[SSIZE];
}PROC;
***************************/
#define NPROC 9
#define NULL 0

PROC proc[NPROC], *running, *freeList, *readyQueue;
int procsize = sizeof(PROC);
int body();

int init()
{
  int i, j; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;

  p = dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0;
  running = p;
  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

void kexit()
{
  printf("proc %d kexit\n", running->pid);
  running->status = FREE;
  running->priority = 0;
  enqueue(&freeList, running); //putproc(running);
  tswitch();
}
  
PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);
  printf("%d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p;
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
      enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
}  

int do_wait()
{
  int pid, status;
  pid = kwait(&status);
  printf("proc %d waited for a ZOMBIE child %d status=%d\n", 
        running->pid, pid, status);
}

/************ (3). Implement kwait() ******************/
int kwait(int *status)
{
    int child_pid;
    PROC * parent = running;
    PROC * temp = running->child;
    PROC * children = NULL; // set new child set to null, eventually fill with nonzombie children

    // if (no child), throw error
    if (!running->child)
        return -1;


    while (1){
        if (temp) {
            // if the child is a ZOMBIE proc
            if(temp->status == ZOMBIE ) {
                parent->child = temp->sibling;

                // kill the zombie
                *status = temp->exitCode;
                temp->sibling = NULL;
                temp->status = FREE;

                // add zombie to the freeList
                enqueue(&freeList, temp);
            }
            // update temp
            temp = temp->sibling;
        }
        // update parent children?
    }
    ksleep(running->pid);
}

int body()
{
  char c; char line[64];
  int pid;
  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    pid = running->pid;
    if (pid==0) color=BLUE;
    if (pid==1) color=WHITE;
    if (pid==2) color=GREEN;
    if (pid==3) color=CYAN;
    if (pid==4) color=YELLOW;
    if (pid==5) color=WHITE;
    if (pid==6) color=GREEN;   
    if (pid==7) color=WHITE;
    if (pid==8) color=CYAN;
    
    printList("readyQueue", readyQueue);
    kprintf("proc %d running: input a char [s|f|q] : ", running->pid);
    c = kgetc(); 
    printf("%c\n", c);

    switch(c){
      case 's': tswitch();           break;
      case 'f': kfork((int)body, 1); break;
      case 'q': kexit();             break;
      case 'w': do_wait();           break;
    }
  }
}
