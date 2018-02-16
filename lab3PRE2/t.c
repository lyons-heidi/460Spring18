/*********** A Multitasking System ************/
#include <stdio.h>
#include "type.h"

// function prototypes
int enqueue(PROC **queue, PROC *p);
PROC *dequeue(PROC **queue);
int printList(char *name, PROC *p);

int kfork();

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;

int ksleep(int event)
{
    PROC *sleepList;

    running->event = event;
    running->status = SLEEP;

    enqueue(&sleepList, running);
    tswitch();               // switch process
}

/************  (1).Implement kwakeup() ***********/
int kwakeup(int event)
{
    PROC *p;
    int i;
    for (i=0; i < NPROC; i++) // iterate thru all procs, check for sleeping status
    {
        p = &proc[i];
        if (p->status == SLEEP && p->event == event){ // if p is sleeping for the event
            p->status = READY;                        // make p READY to run again
            enqueue(&readyQueue, p);                  // enter p into readyQueue
        }
    }
}

int do_switch()
{
   printf("proc %d switching task\n", running->pid);
   tswitch();
   printf("proc %d resuming\n", running->pid);
}

int do_kfork()
{
   int child = kfork();
   if (child < 0)
      printf("kfork failed\n");
   else{
      printf("proc %d kforked a child = %d\n", running->pid, child); 
      printList("readyQueue", readyQueue);
   }
   return child;
}

/************* (2). Implement kexit() ***************/
char *gasp[NPROC]={
  "Oh! I'm dying ....", 
  "Oh! You're killing me ....",
  "Oh! I'm a goner ....", 
  "Oh! I'm mortally wounded ....",
};

int kexit(int exitValue)
{
    int i;
    PROC * p = running;
    PROC * parent = running;

    printf("*************************************\n"); 
    printf("proc %d: %s\n", running->pid, gasp[running->pid % 4]);
    printf("p %d ", p->pid);
    printList("children", p);
    printf("*************************************\n");

    // check for process 1
    if (p->pid==1){
        printf("P1 never dies\n");
        return 0;
    }

    //  1. Erase process user-mode context, e.g. close file descriptors,
    //  release resources, deallocate user-mode image memory, etc.
    //dequeue(&p);

    /* 2. Dispose of children processes, if any */
    if (p->child)
    {
        while (parent->pid != 1) // check to see where P1 is
        {
            parent = parent->parent; // iterate up the tree
        }

        // set p-> parent to p1
        p->child->ppid = 1;
        p->child->parent = parent;

        // append p->children onto parent's children list
        while (parent->child)
        {
           parent = parent->child;
        }
        parent->child = p->child;
        p->child = FREE;
    }

    /* 3. Record exitValue in PROC.exitCode for parent to get */
    p->exitCode = exitValue;

    /* 4. Become a ZOMBIE (but do not free the PROC) */
    p->status = ZOMBIE;

    /* 5. Wakeup parent and, if needed, also the INIT process P1 */
    p->parent->status = READY;

    // grab next process
    enqueue(&readyQueue, p);
    return 1;
} 

int do_exit()
{
  kexit(running->exitCode);
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



int do_wait()
{
  int pid, status;
  pid = kwait(&status);
  printf("proc %d waited for a ZOMBIE child %d status=%d\n", 
        running->pid, pid, status);
}

void showChild()
{
    printList("running->child", running->child);
}

int body()
{
  int c, CR;
  printf("proc %d resume to body()\n", running->pid);
  while(1){
    printf("***************************************\n");
    printf("proc %d running: Parent=%d  ", running->pid, running->ppid);
    showChild();
    printf("input a char [f|s|q|w] : "); // ADD w command
    c = getchar(); CR=getchar();
     
    switch(c){
      case 'f': do_kfork();     break;
      case 's': do_switch();    break;
      case 'q': do_exit();      break;
      case 'w': do_wait();      break;
      default :                 break;  
    }
  }
}

void enter_child(PROC * cur, PROC * child){
    if(!cur->child){
        cur->child = child;
        return;
    }
    else {
        cur = cur->child;
        while(cur->sibling) {
            cur = cur->sibling;
        }
        cur->sibling = child;
        return;
    }
}


/*******************************************************
  kfork() creates a child proc; returns child pid.
  When scheduled to run, child PROC resumes to body();
********************************************************/
int kfork()
{
  PROC *p;
  int  i;
  /*** get a proc from freeList for child proc: ***/
  p = dequeue(&freeList);
  if (!p){
     printf("no more proc\n");
     return(-1);
  }

  /* initialize the new proc and its stack */
  p->status = READY;
  p->priority = 1;         // for ALL PROCs except P0
  p->ppid = running->pid;
  p->parent = running;

  //                    -1   -2  -3  -4  -5  -6  -7  -8   -9
  // kstack contains: |retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
  for (i=1; i<10; i++)
    p->kstack[SSIZE - i] = 0;

  p->kstack[SSIZE-1] = (int)body;
  p->ksp = &(p->kstack[SSIZE-9]); 
 

 //  Implement the process BINARY tree
/**** ADD code to implement process binary tree ********
  
********************************************************/
enter_child(running, p);


  enqueue(&readyQueue, p);
  printList("readyQ", readyQueue);
  return p->pid;
}

int init()
{
  int i; PROC *p;
  for (i = 0; i < NPROC; i++){
    p = &proc[i];
    p->pid = i; 
    p->status = FREE;
    p->priority = 0;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0;
 
  freeList = &proc[0];        
  readyQueue = 0;
  sleepList = 0;

  // create P0 as the initial running process
  p = running = dequeue(&freeList);
  p->status = READY;
  p->priority = 0;
  p->child = 0;
  p->sibling = 0;
  p->parent = running;
  printList("freeList", freeList);
  printf("init complete: P0 running\n"); 
}

/*************** main() ***************/
main()
{
   printf("\nWelcome to Heidi's 460 Multitasking System\n");
   init();
   kfork();  
   printf("P0: switch task\n");
     tswitch();
   printf("All dead. Happy ending\n");
}

/*********** scheduler *************/
int scheduler()
{ 
  printf("proc %d in scheduler()\n", running->pid);
  if (running->status == READY)
      enqueue(&readyQueue, running);
  printList("readyQueue", readyQueue);
  running = dequeue(&readyQueue);
  printf("next running = %d\n", running->pid);  
}
