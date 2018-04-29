
#include "ucode.c"

// fn prototypes TODO put all here
// TODO have redirect check?
char *mystrtok(register char *s, register const char *delim);
void * my_memset(void *s, int c, int n);

void do_pipe(char *cmdl);
void redirect(char *cmd, int redirect_type);

int status;


/* log out of the current user account */
void logout() {
    prints("Heidi Logging Out ~\n");
    chdir("/");
    exit(0);
}


/***************** do_pipe Algorithm *****************/
/*
int pid, pd[2];
pipe(pd); // create a pipe: pd[0]=READ, pd[1]=WRITE
pid = fork(); // fork a child to share the pipe
if (pid){ // parent: as pipe READER
close(pd[1]); // close pipe WRITE end
dup2(pd[0], 0); // redirect stdin to pipe READ end
exec(cmd2);
}
else{ // child : as pipe WRITER
close(pd[0]); // close pipe READ end
dup2(pd[1], 1); // redirect stdout to pipe WRITE end
exec(cmd1);
}
*/

void do_pipe(char *inc_cmd) {
    int pid, pd[2];
    int i=0, j=0;
    char head[128], tail[128], temp[128];
    char *tkn, *cmd1, *cmd2;

    printf("inside do_pipe command. command: [%s]\n", inc_cmd);
    // copy first chunk of command line contents into temp char arr
    
    strcpy(temp, inc_cmd);
    printf("temp value: [%s]\n", temp);

    // tokenize out the first command, store in head
    while(temp[i] != '|'){
        head[i] = temp[i];
        i++;
    }
    // set last value of str to 0
    head[i - 1] = 0;

    printf("head value: [%s]\n", head);

    // grab second part of pipe cmd
    i += 2;
    
    // tokenize out the second command, store in tail
    while( temp[i] != 0 ) {
        tail[j] = temp[i];
        i++;
        j++;
    }
    tail[j] = 0;

    printf("tail value: [%s]\n", tail);

    pipe(pd); // create a pipe: pd[0]=READ, pd[1]=WRITE
    pid = fork(); // fork a child to share the pipe

    // Check if Parent Process
    if (pid){           // parent: as pipe READER
        close(pd[1]);   // close pipe WRITE end
        dup2(pd[0], 0); // redirect stdin to pipe READ end
        
        // check for nested pipes before exec
        if(pipe_exists(tail) == 1) {
            printf("pipe found in tail str: '%s'", tail);
            do_pipe(tail); // recursively handle tail end of pipe
        }
        else {
            // check for redirection
            redirect(tail, get_redirect_type(tail));

            printf("No nested pipes, executing tail: [%s]\n", tail);
            exec(tail);
            printf("Tail executed\n");
        }
    }

    // Check if Child Process 
    else{ // child : as pipe WRITER
        printf("issa child process\n");
        close(pd[0]); // close pipe READ end
        dup2(pd[1], 1); // redirect stdout to pipe WRITE end

        // check for redirection
        redirect(head, get_redirect_type(head));
        printf("back from redirect, executing head: [%s]\n", head);
        exec(head);
        printf("back from exec head\n");
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
// int redirect_exists(char *cmd) {
//     char *ptr = cmd; 
//     while(*ptr) {
//         if (*ptr == '>' || *ptr == '<')
//             return 1;
//         *ptr++;
//     }
//     return 0;
// }


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


// perform redirection based on redirect type in a string
/*
--> 'cat [filename] >> appendFile' will append contents of [filename] at the end of appendFile
--> 'cat [filename] > newFile' will copy the contents of [filename] 
                      and overwrite them into newFIle if it already exists
--> a.out < inFile' will read the inputs from inFile
*/
void redirect(char *cmd, int redirect_type) {
    int i, j, k;
    int fd;
    char temp[128], command[128], filedir[128];
    int cmdLen = my_strlen(cmd);

    // make a copy of inc command string
    my_memset(temp,0,128);
    my_memset(filedir,0,128);
    my_memset(command,0,128);

    strcpy(temp, cmd);

    printf("Inside redirect! Redirect str: %s, type: %d\n", cmd, redirect_type);
    if (redirect_type == 1) { // '>>'
        // grab the command portion of the cmd, store in command arr
        for(i = 0; temp[i] != '>'; i++){
            command[i] = temp[i];
        }
        command[i - 1] = '\0'; // add null terminator
        
        // grab the filedir after bracket, store in firedir arr
        // i + 3 to jump to start of filename
        for(j = i + 3, k = 0; j < cmdLen; j++,k++){
            filedir[k] = temp[j];
        }
        filedir[k] = '\0'; // add null terminator

        // open the file, append command output to end of file
        fd = open(filedir, O_APPEND | O_WRONLY | O_CREAT);
        dup2(fd, 1);
        exec(command);
    }

    if (redirect_type == 2) { // '>'
        // grab the command portion of the cmd, store in command arr
        for(i = 0; temp[i] != '>'; i++){
            command[i] = temp[i];
        }
        command[i - 1] = '\0'; // add null terminator
        
        // grab the filedir after bracket, store in firedir arr
        // i + 2 to jump to start of filename
        for(j = i + 2, k = 0; j < cmdLen; j++,k++){
            filedir[k] = temp[j];
        }
        filedir[k] = '\0'; // add null terminator

        // open the file, then write exec contents to new/existing file
        fd = open(filedir, O_WRONLY | O_CREAT);
        dup2(fd, 1);
        exec(command);
    }
    
    if (redirect_type == 3) { // '<'
        // grab the command portion of the cmd, store in command arr
        for(i = 0; temp[i] != '<'; i++){
            command[i] = temp[i];
        }
        command[i - 1] = '\0'; // add null terminator
        
        // grab the filedir after bracket, store in firedir arr
        // i + 3 to jump to start of filename
        for(j = i + 3, k = 0; j < cmdLen; j++,k++){
            filedir[k] = temp[j];
        }
        filedir[k] = '\0'; // add null terminator

        // open the file, read inputs from file, feed into command
        fd = open(filedir, O_RDONLY);
        dup2(fd, 0);
        exec(command);
    }
    if (redirect_type == 0) {
        printf("no redirects in cmd!\n");
        return;
    }
}

int main(int argc, char *argv[ ])
{
    char command[128], currentdir[128], temp[128];
    int pid, fd; // process id, file descriptor
    char *token;

    while(1) {

        printf("======================================\n");
        printf("= - -- Welcome to Heidi's Shell -- - =\n");
        printf("======================================\n");
        
        getcwd(currentdir);

        // handle user command 
        printf("Heidi-Sh: %s$ ", currentdir); // display cwd
        gets(command);

        strcpy(temp, command); // handle temp
        if(command[0] == 0 || command[0] == ' '){
            continue;
        }

        // parse out the actual command
        token = mystrtok(temp, " ");



        // logout
        if (strcmp("logout", token) == 0 ) {
            logout();
        }

        else if (strcmp("cd", token) == 0 ) {
            // grab filedir
            token = mystrtok(0, " ");

            // handle no filedir given
            if(token == 0) {
                prints("Chdir into current dir\n");
                chdir(currentdir);
            }
            // change dir
            else {
                printf("change dir to = %s\n", token);
                chdir(token);
            }
        }

        // fork a child process! execute commands!
        else { 
            printf("Forking a child process\n");
            pid = fork();

            // check for fork success
            if (pid < 0) {
                printf("Error! No available child processes! Exiting..\n");
                exit(1);
            }

            if (pid) {
                // exec child process first
                printf("Parent id: %d, forked child id: %d", getpid(), pid);
                pid  = wait(&status);
                printf("Child process %d finished\n", pid);
            }

            // execute command and check for piping or for redirect symbols
            else { 
                printf("\n incoming command = %s \n", command);

                //check if incoming command has a redirect:
                if (pipe_exists(command) == 0){
                    redirect(command, get_redirect_type(command));
                    exec(command);
                }
                else{
                    prints("============ PIPING! ============\n");
                    do_pipe(command);
                }
            }
        }
        my_memset(currentdir, 0, 128);
    }


} 