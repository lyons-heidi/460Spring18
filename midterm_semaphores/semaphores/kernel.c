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
int kwait(int *status); // prototype for kwait



/* SEMAPHORE FUNCTIONS */

/* 
Producer operation. Block a process in the semaphore's waiting 
queue. 
*/
int P(struct semaphore *s)
{
  int SR = int_off();
  s->value--;
  if (s->value < 0)
  {
    block(s);
  }
  int_on(SR);
}

/* Release a blocked process, allow it to use a resource. */
int V(struct semaphore *s)
{
  int SR = int_off();
  s->value++;
  if (s->value <= 0)
  {
    signal(s);
  }
  int_on();
}

int block(struct semaphore *s) {
  running->status = BLOCK;
  enqueue(&s->queue, running);
  tswitch();
}

int signal(struct semaphore *s) {
  PROC *p = dequeue(&s->queue);
  p->status = READY;
  enqueue(&readyQueue, p);
}


// ksleep
int ksleep(int event)
{
  int SR = int_off();      // disable IRQ and return CPSR
  running->event = event;
  running->status = SLEEP;
  tswitch();               // switch process
  int_on();                // restore original CPSR
}

//kwakeup
int kwakeup(int event)
{
  int SR = int_off(); // disable IRQ and return CPSR
  PROC *p;
  int i;
  for (i=0; i < NPROC; i++)
  {
    p = &proc[i];
    if (p->status == SLEEP && p->event == event){
      p->status = READY;
      enqueue(&readyQueue, p);
    }
  }
  int_on(SR); // restore original CPSR
}


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

/* implementation of KEXIT */
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
  PROC * p1 = &proc[1];

  // check for process 1
    if (p->pid==1){
      printf("P1 never dies\n");
      return 0;
    }

  /* 2. Dispose of children processes, if any */
  // check if process has a child
  if (p->child){
    printf("p->child check in kexit\n");
      if (p1->child){ // p1 has a child
        PROC * temp = p1;
        while (temp->child){ // get to the end of P1's children
          temp = temp->child;
        }
        temp->child = p->child; // assign P1's child's child .. to proc's child
      }
      else { // p1 has no children, assign proc's child to p1->child
        p1->child = p->child;
      }
    }
  printf("*************************************\n"); 
  printf("proc %d: %s\n", running->pid, gasp[running->pid % 4]);
  printf("p %d ", p->pid);
  printList("children", p->child);
  printf("*************************************\n");


  /* 3. Record exitValue in PROC.exitCode for parent to get */
  p->exitCode = exitValue = p->pid;

  /* 4. Become a ZOMBIE (but do not free the PROC) */
  p->status = ZOMBIE;

  /* 5. Wakeup parent and, if needed, also the INIT process P1 */
  p->parent->status = READY; //TODO: kwakeup

  kwakeup(p->ppid); // wakeup the parent
  tswitch();
  // grab next process
  enqueue(&readyQueue, running);
  return 1;
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
  p->parent = running;

  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);

  //  Implement the process BINARY tree
  enter_child(running, p);

  enqueue(&readyQueue, p);

  printf("%d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  printList("readyQ", readyQueue);

  // Print child list of running
  printList("running->child", running->child);
  return p;
}

/*******************************************************
  kfork() creates a child proc; returns child pid.
  When scheduled to run, child PROC resumes to body();
********************************************************/
// int kfork()
// {
//   PROC *p;
//   int  i;
//   /*** get a proc from freeList for child proc: ***/
//   p = dequeue(&freeList);
//   if (!p){
//      printf("no more proc\n");
//      return(-1);
//   }

//   /* initialize the new proc and its stack */
//   p->status = READY;
//   p->priority = 1;         // for ALL PROCs except P0
//   p->ppid = running->pid;
//   p->parent = running;

//   //                    -1   -2  -3  -4  -5  -6  -7  -8   -9
//   // kstack contains: |retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
//   for (i=1; i<10; i++)
//     p->kstack[SSIZE - i] = 0;

//   p->kstack[SSIZE-1] = (int)body;
//   p->ksp = &(p->kstack[SSIZE-9]); 
 

//  //  Implement the process BINARY tree
// /**** ADD code to implement process binary tree ********
  
// ********************************************************/
// enter_child(running, p);


//   enqueue(&readyQueue, p);
//   printList("readyQ", readyQueue);
//   return p->pid;
// }



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
        if (temp) { // loop thru 
            // if the child is a ZOMBIE proc
            if(temp->status == ZOMBIE ) {
                parent->child = temp->sibling;

                // kill the zombie
                *status = temp->exitCode;
                temp->sibling = NULL;
                temp->status = FREE;

                // add zombie to the freeList
                enqueue(&freeList, temp);
                return temp->pid;
            }
            // update temp
            temp = temp->sibling;
        }
        // update parent children?
        ksleep(running->pid);
        
    }
    
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
    kprintf("proc %d running: input a char [s|f|q|w] : ", running->pid);
    c = kgetc(); 
    printf("%c\n", c);

    switch(c){
      case 's': tswitch(); printSiblings("Process' child list", running->child);   break;
      case 'f': kfork((int)body, 1);      break;
      case 'q': kexit(running->exitCode); break;
      case 'w': do_wait();                break;
      case 'p': printList("FreeList", freeList); printList("Running", running); break;
    }
  }
}
