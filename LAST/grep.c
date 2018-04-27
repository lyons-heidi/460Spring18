/********** GREP FILE *************/
#include "ucode.c"


char *getsubstr(char *substr, char *str);
void * my_memset(void *s, int c, int n);



int main(int argc, char *argv[ ])
{
    int fd; // file descriptor!
    int n, line;
    int lenline = 80; // handle at most 80 lines
    char pattern[64], buf[128];

    int pid = getpid();
    printf("Heidi-sh: proc %d running GREP program\n", pid);

    printf("argc = %d\n", argc);
    for (i=0; i<argc; i++){
        printf("argv[%d] = %s\n", i, argv[i]);
    }


    // handle 'grep'
    if (argc <=1) {
        printf("grep error! More arguments needed\n");
        exit(1);
    }

    // handle 'grep KEYWD'
    else if(argc == 2){
        // store pattern
        strcpy(pattern, argv[1]);
        fd = dup(0);

        // go through the entire dir's 'file' until EOF
        while( E_OF == 1 ) {
            // clear bufer
            my_memset(buf, 0, 128);

            // go through a line and see if 'pattern' matches:
            line = 0;
            while(line < lenline){     // a typical line is 80 chars long
                // read in a line to the buffer
                n = read(fd, &buf[line], 1);
                
                // end of file reached:
                if(n == 0) {
                    E_OF = 0;
                }

                // end of line reached
                else if ( buf[line] == '\n' || buf[line] == '\r' ) {
                    buf[line + 1] = 0;
                    break;
                }
                line++;
            }

            printf("%s", buf); //testing buffer.
            
            // pattern found! Print to console
            if(getsubstr(pat, buf) != 0) {
                printf("%s", buf);
            }
        }

        // close buffer at end
        close(fd);
    }

    // handle 'grep KEYWD filename'
    else if(argc == 3) {
        strcpy(pattern, argv[1]);

        // try to open specified file
        if( (fd = open(argv[2], O_RDONLY)) < 0 ) {
            prints("Error! Unable to open file, exiting..\n");
            exit(1);
        }

        // go through the entire file until EOF
        while( E_OF == 1 ) {
            // clear bufer
            my_memset(buf, 0, 128);

            // go through a line and see if 'pattern' matches:
            line = 0;
            while(line < lenline){     // a typical line is 80 chars long
                // read in a line to the buffer
                n = read(fd, &buf[line], 1);
                
                // end of file reached:
                if(n == 0) {
                    E_OF = 0;
                }

                // end of line reached
                else if ( buf[line] == '\n' || buf[line] == '\r' ) {
                    buf[line + 1] = 0;
                    break;
                }
                line++;
            }

            printf("%s", buf); //testing buffer.
            
            // pattern found! Print to console
            if(getsubstr(pat, buf) != 0) {
                printf("%s", buf);
            }
        }
        close(fd);
    }
    printf("exit\n");
}


/* Find the substring in a given str, if it exists */
char *getsubstr(char *substr, char *str){
  char *a , *b;

  b = substr;
  if(*b == 0){
      return str;
  }
  for( ; *str != 0; str++){
      if(*str != *b){
          continue;
      }
      a = str;
      while(1){
          if (*b == 0){
              return str;
          }
          if (*a++ != *b++){
              break;
          }

      }
      b = substr;
  }
  return 0;
}