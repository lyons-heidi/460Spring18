/********************* cat.c file ****************/

#include "ucode.c"

void * my_memset(void *s, int c, int n);
int io = 0;

int main (int argc, char *argv[]) {

    int n = 0;
    int fd; 
    int stdin = 0, stdout = 1; // read/write to stdin/out
    char buf[512], cmdLine[64], user_ch;
    
    //prints("********** Heidi's Cat MEOW ************ \n");

    // make sure user enters input
    if(argc <= 0){
        printf("Error, no input found! \n");
        exit(1);
    }

    // only one argument given to main, get input from stdin
    else if (argc == 1) {
        fd = 0;
        while(read(0, &user_ch, 1)){
            // If newline detected, write to screen
            if(user_ch == '\n' || user_ch == '\r'){
                write(1, '\n', 1);
                //write(1, "\r", 1);
            }

            // write char to screen
            else{
                write(1, &user_ch, 1);
            }
        }
    }

    else{
        // open file, read and print the buffer
        fd = open(argv[1], O_RDONLY);
        my_memset(buf, 0, 512);

        while((n = read( fd, buf, 512))){
            prints(buf);
            my_memset(buf, 0, 512);
        }
        close(fd);
    }
}