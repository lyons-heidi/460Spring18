#define NPROC     9
#define SSIZE  1024

#define FREE      0        // proc status
#define READY     1 
#define ZOMBIE    2
#define SLEEP     3

// #define NULL 0

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct proc{
    struct proc *next;      // next proc pointer:        0
    int  *ksp;              // do NOT alter this field:  4 

    int   pid;              // pid = 0 to NPROC-1
    int   ppid;             // parent pid 

    int   status;           // PROC status 
    int   priority;         // scheduling priority 
    int   event;            // event to sleep on 
    int   exitStatus;       // exit code value

    struct proc *child ;    // first child PROC pointer
    struct proc *sibling;   // sibling PROC pointer
    struct proc *parent;    // parent PROC pointer
    
    int   kstack[SSIZE];    // processs stack                 
}PROC;

PROC *get_proc();
PROC *dequeue();