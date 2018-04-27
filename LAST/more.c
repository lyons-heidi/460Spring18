/********** test.c file *************/
#include "ucode.c"



// fn prototypes
void printLine(int fd);
void printChar(int fd);
void printPage(int fd);

void outprintLine(int fd);
void outprintChar(int fd);
void outprintPage(int fd);

int EndOF = 1;

int main(int argc, char *argv[ ])
{
    int n = 0, linelen = 80, page = 20;
    int fd; // file descriptor!
    int stdin = 0, stdout = 1;
    char buf[1600], cmd[64], userch, c;


    int pid = getpid();
    printf("Heidi-sh: proc %d running MORE program\n", pid);

    if (argc == 1) {
        printf("no file selected!\n");

        fd = dup(0);    // move the current read somewhere else:
        close(0);       // close read and open it for the terminal
        char tty[128];
        gettty(tty);    // store the terminal's file 
        open(tty, 0);   // open terminal for read.

        EndOF = 1;

        outprintPage(fd);

        while( EndOF != 0 ) { 
            // respond to user input
            char input = getc();

            // if enter key pressed
            if( input == '\r' ) {
                outprintLine(fd);
            }

            // if space bar pressed
            else if( input == ' ') {
                outprintPage(fd);
            }

            // anything else pressed
            else {
                outprintChar(fd);
            }
        }
        close(fd); 
    }

    else { // print contents of a file

        fd = open(argv[1], 0);

        printPage(fd);
        while( EndOF != 0 ) { 
            // respond to user input
            char input = getc();

            // if enter key pressed
            if( input == '\r' ) {
                printLine(fd);
            }

            // if space bar pressed
            else if( input == ' ') {
                printPage(fd);
            }

            // anything else pressed
            else {
                printChar(fd);
            }
        }
        close(fd); 
    }
}


/* Print per line from a file */
void printLine(int fd){
    int byte = 0, n = 0;
    int linelen = 80;
    char c;

    // read 80 bytes in a line
    for(byte = 0; byte < linelen; byte++){
        // read in the char:
        n = read(fd, &c, 1);
        
        // check for EndOF
        if(n == 0){
           EndOF = 0;
           return; 
        }

        mputc(c); 

        // if the current char is a newline, break
        if (c == '\n') { break; }
    }

}


/* Print per char from a file */
void printChar(int fd) {
    char c;
    int n = read(fd, &c, 1);
    
    // if no character was read in
    if(n == 0){
        EndOF = 0;
        return;
    }

    // print char to screen, unless its a newline or return key
    if(c != '\n' || c != '\r'){
        mputc(c);
    }
}



/* Print per page (20 lines at a time) from a file */
void printPage(int fd) {
    int line = 0, byte = 0, n = 0;
    int numlines = 20, numchars = 80;
    char c;

    // Print 20 lines at a time to equal a 'page'
    while(line < numlines){
        byte = 0;

        // read 80 bytes in a line:
        while(byte < numchars){
            // read in the char:
            n = read(fd, &c, 1);
            
            // end of file reached?
            if(n == 0) {
                EndOF = 0;
                return;
            }
            if(c != '\n' || c != '\r'){
                mputc(c);
            }

            // incr to next byte
            byte++;
        }
        // incr to next line
        line++;
    }

    // add newline at very end
    mputc('\n');
}


/* Print per line from stdin */
void outprintLine(int fd) {
    int byte = 0, n = 0;
    int linelen = 80;
    char c;

    // read 80 bytes in a line
    while(byte < linelen) {
        // read in the char:
        n = read(fd, &c, 1);
        
        // check for EndOF
        if(n == 0){
           EndOF = 0;
           return; 
        }

        write(1, &c, 1); 

        // if the current char is a newline, break
        if (c == '\n') { break; }
        byte++;
    }
}


/* Print per char from stdin */
void outprintChar(int fd) {
    char c;
    int n = read(fd, &c, 1);
    
    // if no character was read in
    if(n == 0){
        EndOF = 0;
        return;
    }

    // print char to screen, unless its a newline or return key
    if(c != '\n' || c != '\r'){
        write(1, &c, 1);
    }
}


/* Print per page (20 lines at a time) from stdin */
void outprintPage(int fd) {
    int line = 0, byte = 0, n = 0;
    int numlines = 20, numchars = 80;
    char c;

    // Print 20 lines at a time to equal a 'page'
    while(line < numlines){
        byte = 0;

        // read 80 bytes in a line:
        while(byte < numchars){
            // read in the char:
            n = read(fd, &c, 1);
            
            // end of file reached?
            if(n == 0) {
                EndOF = 0;
                return;
            }
            if(c != '\n' || c != '\r'){
                write(1, &c, 1);
            }

            // incr to next byte
            byte++;
        }
        // incr to next line
        line++;
    }

    // add newline at very end
    mputc('\n');
}