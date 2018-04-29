/********************* LOW TO HIGH CODE ****************/
#include "ucode.c"


// function prototypes
void * my_memset(void *s, int c, int n);
char *toUpper(char *c);


/* Given a string, perform l2u and return updated string*/
// Check ASCII characters for a-z (97-122)
char *toUpper(char *str){
  int i = 0;
  while(*str){
      if(str[i] <= 122 && str[i] >= 97){
          str[i] = str[i] - 32;
      }
      str++;
  }
  return str;
}



int main (int argc, char *argv[]) {
    prints("********** Heidi's L2U Command ************ \n");
    
    int n = 0, m = 0;
    int fds, fdd; // TODO UNDERSTAND
    int stdin = 0, stdout = 1; // read/write to stdin/out
    char buf[512], user_ch;
    
    printf("argc: %d\n", argc);

    // make sure user enters input
    if(argc < 0){
        prints("Error, no input found! \n");
        exit(1);
    }

    //make things uppercase:
    if(argc == 1){
        fds = 0;
        fdd = 1;
        //prints("No filename given!\n");
        my_memset(buf, 0, 512);
        while((n = read(fds, buf, 1)) > 0){
            if(n < 0) { 
                break;
            }
            //check for '\n':
            toUpper(buf);
            m = 0;
            while(m < n) {
                write(fdd, &buf[m], 1);
                m++;
            }
        }
        close(fds);
        close(fdd);
        exit(1);

    }
    
    // Display the contents of a single file in uppercase
    else if(argc == 2){
        fds = open(argv[1], O_RDONLY);
        fdd = 1;
        
        my_memset(buf, 0, 512); // clear the buffer


        while ( (n = read(fds, buf, 1) ) > 0) {
            if( n < 0 ) { 
                break;
            }


            //check for '\n' character:
            toUpper(buf);

            m = 0;
            while(m < n) {
                if(buf[m] == '\r'){
                    buf[m] = '\n';
                }
                
                write(fdd, &buf[m], 1);

                if(buf[m] == '\n') { 
                    write(2, "\r", 1);
                }
                m++;
            }
        }

        // close file descriptors
        close(fds);
        close(fdd);
        exit(1);
    }

    else{ // two files or long command

        fds = open(argv[1], O_RDONLY);
        fdd = open(argv[2], O_WRONLY | O_CREAT);

        if( fds < 0 || fdd < 0 ) {
            prints("open() failed");
        }

        else{
            // Read in the file per char and convert to UPPER
            while( n = read(fds, buf, 512) ) {

                //check for '\n':
                toUpper(buf);
                m = 0;
                
                while(m < n){
                    if(buf[m] == '\r' || buf[m] == '\n'){
                        buf[m] = '\n';
                    }
                    
                    write(fdd, &buf[m], 1);
                    m++;
                }
            }
            prints("Finished with L2U\n");
            close(fds);
            close(fdd);
        }
    }

    exit(1);
}