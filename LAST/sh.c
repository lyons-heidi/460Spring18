#include "ucode.c"

// fn prototypes
char *mystrtok(char *source, const char *delimeter);
void * my_memset(void *s, int c, int n);

void do_pipe(char *cmdl);

int status;


/* log out of the current user account */
void logout() {
    prints("Heidi Logging Out ~\n");
    chdir("/");
    exit(0);
}


/***************** do_pipe Algorithm *****************/
void do_pipe(char *inc_cmd) {
    int pid, pd[2];
    int i=0, j=0;
    char head[128], tail[128], temp[128];
    char *tkn, *cmd1, cmd2;

    // copy first chunk of command line contents into temp char arr
    strcpy(temp, inc_cmd);

    // tokenize out the first command, store in head
    while(temp[i] != '|'){
        head[i] = temp[i];
        i++;
    }
    // set last value of str to 0
    head[i - 1] = 0;

    // grab second part of pipe cmd
    i += 2;
    
    // tokenize out the second command, store in tail
    while( temp[i] != 0 ) {
        tail[j] = temp[i];
        i++;
        j++;
    }
    tail[j] = 0;

    pipe(pd); // create a pipe: pd[0]=READ, pd[1]=WRITE
    pid = fork(); // fork a child to share the pipe

    // Check if Parent Process
    if (pid){ // parent: as pipe READER
        close(pd[1]); // close pipe WRITE end
        dup2(pd[0], 0); // redirect stdin to pipe READ end
        
        // check for nested pipes before exec
        if(pipe_exists(tail) == 1) {
            printf("pipe found in tail str: '%s'", tail);
            do_pipe(tail); // recursively handle tail end of pipe
        }
        else {
            // TODO: check for redirection here
            exec(tail);
        }
    }

    // Check if Child Process 
    else{ // child : as pipe WRITER
        close(pd[0]); // close pipe READ end
        dup2(pd[1], 1); // redirect stdout to pipe WRITE end
        // TODO: check for redirection here
        exec(cmd1);
    }
    /* Multiple pipes are handled recursively, from right to left. */
    return;
}


/* Check to see if a pipe character exists in an inc string
--> return 1 if pipe exists in incoming cmd
--> return 0 if no pipes found  */
int pipe_exists(char *cmd) {
    char *ptr = cmd;
    while(*ptr) {
        if (*ptr == '|')
            return 1;
        *ptr++;
    }
    return 0;
}

/* Check to see if a redirect char exists in an inc string
--> return 1 if '<' or '>' exists in incoming cmd
--> return 0 if no redirect char found  */
int redirect_exists(char *cmd) {
    char *ptr = cmd; 
    while(*ptr) {
        if (*ptr == '>' || *ptr == '<')
            return 1;
        *ptr++;
    }
    return 0;
}


/* Check to see which redirect char exists in an inc string
--> return 1 if '>>' found in incoming cmd
--> return 2 if '>'  found in incoming cmd
--> return 3 if '<'  found in incoming cmd 
--> return 0 if no redirects found        */
int get_redirect_type(char *cmd) {
    char *ptr = cmd;
    while(*ptr) {
        if (*ptr == '>') {
            // check if single or double
            *ptr++;
            if (*ptr == '>') {
                // double found
                return 1;
            }
            // just the single found
            return 2;
        }
        if (*ptr == '<'){
            return 3;
        }
        *ptr++;
    }
    return 0; // no redirection chars found
}

void redirect(char *cmd) {
    int redirect_type = get_redirect_type(cmd);
    if (redirect_type == 1) { // '>>'

    }

    if (redirect_type == 2) { // '>'
        
    }
    
    if (redirect_type == 3) { // '<'
        
    }
}

int main(int argc, char *argv[ ])
{
  
}