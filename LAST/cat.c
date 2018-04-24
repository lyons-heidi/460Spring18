/********************* cat.c file ****************/

#include "ucode.c"

void * my_memset(void *s, int c, int n);


int main (int argc, char *argv[]) {
    prints("********** Heidi's Cat MEOW ************ \n");
    
    int n = 0;
    int fd; // file descriptor
    int stdin = 0, stdout = 1; // read/write to stdin/out
    char buf[1024], cmdLine[64], user_ch;
    
    // make sure user enters input
    if(argc < 0){
        prints("Error, no input found! \n");
        exit(1);
    }

    // Only one ch found
    else if (argc == 1) {
        fd = 0;
        prints("Error! User must specify a file!\n");
        // while(read(0, &user_ch, 1)){
        //     // If newline detected, write to screen
        //     if( user_ch == '\n' || user_ch == '\r' )
        //     {
        //         write(1, "\n", 1);
        //         write(1, "\r", 1);
        //     }

        //     // write char to screen
        //     else{
        //         write(1, &user_ch, 1);
        //     }
        // }
    }

    else{
        // open file, read and print the buffer
        fd = open(argv[1], O_RDONLY);
        my_memset(buf, 0, 1024);
        while((n = read( fd, buf, 1024))){
            prints(buf);
            my_memset(buf, 0, 1024);
        }
        close(fd);
    }
}