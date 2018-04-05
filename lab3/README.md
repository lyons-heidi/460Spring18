Written by Heidi Lyons on 4-4-2018
Existing code taken from c7.4.forkexec.tgz written by KC Wang


LAB Assignment #3: DUE 2-21-2018

Modify the example program C5.3 by adding 

       int event;             // sleep event
       int ppid;              // parent pid
       int exitCode;          // exit value 
       struct proc *child;
       struct proc *sibling;
       struct proc *parent;

to the PROC structure.

(1). Implement parent-child process relation and process BINARY tree

(2). Implement sleep()/wakeup() as in Text

(3). Implement kexit(int exitValue) for process (except P1) to terminate
        
(4). Implement 

       int pid = wait(int *status)

     to allow parent to wait for child termination as in PreLab3
