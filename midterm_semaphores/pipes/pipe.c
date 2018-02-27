

/*
typedef struct pipe{
    char buf[PSIZE]; // circular data buffer
    int head, tail; // circular buf index
    int data, room; // number of data & room in pipe
    int status; // FREE or BUSY
}PIPE;
 */

int pipe_init(){
    printf("inside pipe init\n");
    int i = 0;
    for(i=0; i<NPIPE; i++){
        // pipe[i].data = 8;
        // pipe[i].head = 0, pipe[i].tail = 0;
        // pipe[i].room = 0;
        pipe[i].status = FREE;
        return 1;
    }
    return 0;
}

/* Creates a PIPE object in the (shared) address space 
   of all the processes. It allocates a free PIPE object, 
   initializes it and returns a pointer to the created PIPE object.
*/
PIPE *create_pipe(){
    printf("Creating pipe.. ");
    int i = 0, j = 0;
    for (i = 0; i < NPIPE; i++){
        if(pipe[i].status == FREE) {
            // initialize FREE pipe to zero
            for(j=0;j<PSIZE;j++){
                pipe[i].buf[j] = 0;
            }
            printf("OK. Using pipe %d\n", i);
            pipe[i].head = pipe[i].tail = 0;
            pipe[i].data = 0;
            pipe[i].room = 8;
            pipe[i].status = BUSY; // set to busy (maybe)
            return &pipe[i];
        }
        return 0;
    }

}

/* Deallocate the PIPE object, wake up all the sleeping processes on the pipe. */
// int destroy_pipe(PIPE *pipePtr){
//     int i = 0, j = 0;
//     for (i = 0; i < NPIPE; i++){

//     }
// }


/*---------- Algorithm of pipe_read -------------*/
int read_pipe(PIPE *p, char *buf, int n){
    int r = 0;
    if (n<=0)
        return 0;
    
    // validate PIPE pointer p; 
    if (p->status == FREE) // p->status must not be FREE
        return 0;

    while(n){
        while(p->data){
            *buf++ = p->buf[p->tail++]; // read a byte to buf
            p->tail %= PSIZE;
            p->data--; p->room++; r++; n--;
            if (n==0){
                break;
            }
        }
        kwakeup(&p->room); // wakeup writers
        if (r){           // if has read some data
            return r;
        }
        // pipe has no data
        ksleep(&p->data); // sleep for data
    }
}


/*---------- Algorithm of write_pipe -----------*/
int write_pipe(PIPE *p, char *buf, int n){ 
    int r = 0;
    if (n<=0)
        return 0;

    //validate PIPE pointer p; 
    if (p->status == FREE) // p->status must not be FREE
        return 0;

    while(n){
        while(p->room){
            p->buf[p->head++] = *buf++; // write a byte to pipe;
            p->head %= PSIZE;
            p->data++; p->room--; r++; n--;
            if (n==0)
                break;
        }
        kwakeup(&p->data); // wakeup readers, if any.
        if (n==0){
            if(p->data) 
                tswitch();
            return r; // finished writing n bytes
        }
            
        
        // still has data to write but pipe has no room
        ksleep(&p->room); // sleep for room
    }
}