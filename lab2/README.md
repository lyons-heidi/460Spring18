Written by Heidi Lyons on 4-4-2018
Existing code taken from c7.4.forkexec.tgz written by KC Wang


               CS460 LAB Assignment #2
            
                 DUE & DEMO: 2-1-2018

		

1. The UART driver provides ugetc(UART *up), which inputs on char from the uart
   pointed by up.

Write YOUR OWN ufprinf(UART *up, char *fmt, ....) function for formatted print 
to a uart pointed by up.

HELP: for fucnctions with 3 dots, the gcc compiler generated code passes all 
      parameters on stack. Dig out your 360 assignments to see how did you 
      implemented myprintf(char *fmt, ...) before.


2. The LCD driver can display BMP files. 

2.1. Display YOUR own pretty picture in .bmp file format

2.2. Download samples/LAB2/wsu.bmp. Dispaly it in VGA mode (640x480) resolution.


    