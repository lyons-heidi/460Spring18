Written by Heidi Lyons on 4-4-2018
Existing code taken from c7.4.forkexec.tgz written by KC Wang

                  460 Assignment
           DUE & DEMO: Thursday 3-22-2018

Demo Schedule

1. Per the sample code of c7.1 in the book, ANSWER THE FOLLOWING:

(1). The initial page table is at 16KB. What are the page table contents?
     What's the virtual space range?

     Sample ANS: Draw a diagram to show
            entries 0-257 contains what? Other entries contain what?
     Based on the entries, what are the virtual address range?

(2). Each process Pi (i >= 0) has a page table at 6MB + i*16K.
     What are the page table entries of P0?

     Each Pi (i > 0) has a 1MB Umode image at 8MB + (i-1)*1MB.
     What are the page table entries of Pi (i > 0)?

(3). When create a process Pi, we must load its umode image (u1.o) to its Umode
     area. How is this done in c7.1?

(4). When a process Pi issues a system call from Umode, e.g.

        int r = syscall(0, 0, 0, 0);   // int r = getpid();

     Write paragraphs in words to describe the control flow from syscall() 
     to the process return to Umode with the return value r.
   
   Sample answer:

   1.syscall() in C => syscall: in us.s: swi #0 => offset 0x8 in vector_table
   2.CPU to svc_entry: in SVC mode in s.s

   3.follow svc_entry code to describe what happens:
       where are the process Umode r0-r14 saved?
       where are the process Umode upc, usp, ucpsr saved?
   4.Contiue to describe where does the procee go and HOW to get back to Umode.


2. Modify the c7.1 code to do the following:

(1). The same Umode image u1.o for all processes, 
     but each Pi (i > 0) has a 4MB Umode image at 8MB + (i-1)*4MB.

(2). Add a syscall
           char myname[64];
           int r = getname(char myname);
     which returns the process name in myname[ ].
     print the process name in Umode
    
