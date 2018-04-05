Written by Heidi Lyons on 4-4-2018
Existing code taken from c7.4.forkexec.tgz written by KC Wang

               
===========================================================================
                 CS460 LAB1 Check List

1. Does it boot up the MTX OS?______________________________________________

2 For those who can't demo the complete LAB1:

2-1. Show LAB1.1: can boot up and gets()/prints()__________________________

2-2. Show LAB1.2: can display the file names under / directory?____________


============================================================================

                 CS460 LAB#1 QUESTIONS

1. On Intel x86 based PCs,druing booting, what does BIOS do?_________________
   How many sectors does BIOS load from the boot device?_____________________
   Where in memory does BIOS load the booter?________________________________

2. BIOS loads only 512 bytes of a booter into memory, which is only the 
   beginning part of the booter. How does the booter load the remaining part of 
   the booter into memory?__________________________________________________

3. Assume a COMPLETE booter is loaded at the segment 0x9000. WHY do we have to
   set the CPU's sgement registers CS,DS,SS,ES to 0x9000?______________________

4. How do you find the file /boot/mtx?

5. How to load the (disk) blocks of /boot/mtx to the segment 0x1000?__________