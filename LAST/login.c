/********************* login.c file ****************/
/*
LOGIN: Process 2 
The file /dev/tty0 must exist
P2 opens /dev/tty0 as stdin, stdout, stderr then displays login
when user logs in, 
    it reads username and pw from stdin
    open the /etc/passwd file to authenticate the user
    format: 
        username:password:gid:uid:fullname:HOMEDIR:program
    if user has valid combo, take the user's uid and P2 will become that user's process
    Change dir into user's HOMEDIR and execute the listed program, which is normally /bin/sh
    then in sh, loop forever

*/

#include "ucode.c"

char fusername[128], fpassword[128], fgid[10], fuid[10], fullname[128], homedir[256], program[256];
char *mystrtok(char *source, const char *delimeter);

int main(int argc, char *argv[ ])
{
    int in, out, err;
    int success = 0; // login success
    char *token;
    char username[128], password[128]; // for storing user inputted username/pw values
    int uid, gid;
    

    // P2 opens /dev/tty0 as stdin, stdout, stderr then displays login

    // (1) close fd's for stdin, stdout, stderr
    close(0);
    close(1);
    close(2);

    // (2). open argv[1] 3 times as in(0), out(1), err(2).
    in = open(argv[1], 0);
    out = open(argv[1], 1);
    err = open(argv[1], 2);

    // set tty name string in PROC.tty
    fixtty(argv[1]);

    prints("Heidi's Login ~\n");

    while(1){

        int fd, fdLen;
        char buf[256];

        // grab login details
        prints("Heidi-Login: ");
        gets(username);

        prints("Heidi-Password: ");
        gets(password);

        // (4). open /etc/passwd file for READ;
        

        if((fd = open("/etc/passwd", 0))< 0){
            prints("Error! Cannot open() '/etc/passwd'\n");
            exit(1);
        }

        // now read file descriptor:
        if((fdLen = read(fd, buf, 256))< 0){
            prints("Error! Cannot read() '/etc/passwd'\n");
            exit(1);
        }

        /*
        username:password:gid:uid:fullname:HOMEDIR:program
        root:12345:0:0:super user:/:sh
        kcw:abcde:1:1:kcw user:/user/kcw:sh
        guest:mercy:2:2:guest user:/user/guest:sh
        buster:123:3:3:buster dog:/user/buster:sh
        */

        // for each line in /etc/passwd file do{
        // tokenize user account line;
        token = mystrtok(buf, ":\n");
        // printf("buf: [%s]\n", buf);
        // printf("token: [%s]\n", token);
        // printf("username: [%s]\n", username);

        while(token != 0 && success == 0)
        {
            // (6). if (user has a valid account)
            if (strcmp(token, username) == 0) { // usernames match, check password
                // username matches, go onto check if the password matches
                // printf("usernames match!\n");
                strcpy(fusername, username);
                strcpy(fpassword, mystrtok(0, ":\n"));

                if (strcmp(fpassword, password) == 0) {
                    // account has been validated!
                    prints("account validated!\n");
                    success = 1;

                    // cast to integer type!
                    gid = atoi(mystrtok(0, ":\n"));
                    uid = atoi(mystrtok(0, ":\n"));

                    //strcpy(fgid, mystrtok(0, ":\n"));
                    //strcpy(fuid, mystrtok(0, ":\n"));
                    strcpy(username, mystrtok(0, ":\n"));
                    strcpy(homedir, mystrtok(0, ":\n"));
                    strcpy(program, mystrtok(0, ":\n"));

                    // (7). change uid, gid to user's uid, gid; // chuid()
                    chuid(uid, gid);

                    // change cwd to user's home DIR // chdir()
                    chdir(homedir);

                    printf("Login Success! Entering Heidi's shell.\n");

                    // (8). exec to program in user account // exec()
                    exec("sh");

                    // close opened /etc/passwd file // close()
                    close(fd);
                }
            }

            // grab next user information 
            token = mystrtok(0, ":\n");
            
        }
        prints("Login failed, try again\n");

    }
    


}

