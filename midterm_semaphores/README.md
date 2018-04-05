             CS460 Takehome Midterm Exam
  
           DUE: Tuesday, 2-27-2018 CLASS TIME

==================================================================
1. Section 5.9:        Semaphores
   Section 5.10.5.1:   Producer-Consumer Problem using semaphores

                    Requirements:

   (1). Implement P, V operations on semaphores.
   (2). Implement producer-consumer problem using semaphores:

        Define buffer size N=8 (chars);
        Let P0 create a producer task P1; with priority = 1
                      a consumer task P2; with priority = 1
       
        P1: get a line of chars from KBD, e.g. abcd; 1234567890; etc.
            write all chars to buffer; REPEAT 
     
        P2: get chars from buffer; print to LCD screen; REPEAT


======================================================================
2. Section 5.13.2 describes PIPEs in Unix/Linux, and algorithms of 
   pipe_read()/pipe_write using sleep/wakeup for process synchronization.

   Section 5.13.2.3. Demonstrates PIPE in the Textbook.

                    Requirements

   Complete the sample program C5.6 as described in the Text.
=====================================================================

Demonstrate YOUR programs to TA on Tuesday 2-27-2018 (in class)