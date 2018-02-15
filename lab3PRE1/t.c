/*********** A Multitasking System ************/
#include <stdio.h>
#include "type.h"

PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;

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

char *gasp[NPROC]={
  "Oh! I'm dying ....", 
  "Oh! You're killing me ....",
  "Oh! I'm a goner ....", 
  "Oh! I'm mortally wounded ....",
};

int kexit(){
   printf("*************************************\n"); 
   printf("proc %d: %s\n", running->pid, gasp[running->pid % 4]);
   printf("*************************************\n");
   running->status = FREE;
   running->priority = 0;
   enqueue(&freeList, running);
   tswitch();
}

int do_exit()
{
  kexit();    // journey of no return 
}

int body()
{
  int c, CR;
  printf("proc %d starts from body()\n", running->pid);
  while(1){
    printf("***************************************\n");
    printf("proc %d running: parent = %d\n", running->pid, running->ppid);
    printf("input a char [f|s|q] : ");
    c = getchar(); CR=getchar(); 
    switch(c){
      case 'f': do_kfork();     break;
      case 's': do_switch();    break;
      case 'q': do_exit();      break;
      default :                 break;  
    }
  }
}

/*******************************************************
  kfork() creates a child porc; returns child pid.
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
     return -1;
  }

  /* initialize the new proc and its kstack */
  p->status = READY;
  p->priority = 1;         // for ALL PROCs except P0
  p->ppid = running->pid;
  p->parent = running;

  //                    -1   -2  -3  -4  -5  -6  -7  -8   -9
  // kstack contains: |retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
  for (i=1 ; i < 10; i++)
    p->kstack[SSIZE - i] = 0;
  p->kstack[SSIZE-1] = (int)body;
  p->ksp = &(p->kstack[SSIZE-9]);  // changed from p->saved_sp to p->ksp
 
  enqueue(&readyQueue, p);
  return p->pid;
}

int init()
{
  int i;
  for (i = 0; i < NPROC; i++){
    proc[i].pid = i; 
    proc[i].status = FREE;
    proc[i].priority = 0;
    proc[i].next = (PROC *)&proc[(i+1)];
  }
  proc[NPROC-1].next = 0;
 
  freeList = &proc[0];        
  readyQueue = 0;
  sleepList = 0;

  // create P0 as the initial running process
  running = dequeue(&freeList);
  running->status = READY;
  running->priority = 0;  
  running->parent = running;

  printf("init complete: P0 running\n"); 
  printList("freeList", freeList);
}

/*************** main() ***************/
int main()
{
   printf("\nWelcome to 360 Multitasking System\n");
   init();     // initialize freeList, readyQueue; create P0 as running
   kfork();    // P0 kfork P1 in readyQueue
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



