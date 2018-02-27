/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include "type.h"
#include "string.c"
#include "queue.c"
#include "kbd.c"
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
#include "uart.c"
#include "pipe.c"

PIPE *kpipe; // global PIPE pointer

/* Pipe Goodies */
int pipe_writer(){ // pipe writer task code
    struct uart *up = &uart[0];
    char line[128];
    while(1){
        uprintf("Enter a line for task1 to get : ");
        printf("task%d waits for line from UART0\n", running->pid);
        
        ugets(up, line);
        uprints(up, "\r\n");
        printf("task%d writes line=[%s] to pipe\n", running->pid, line);
        write_pipe(kpipe, line, strlen(line));
    }
}

int pipe_reader(){ // pipe reader task code
    char line[128];
    int i, n;
    while(1){
        printf("task%d reading from pipe\n", running->pid);
        n = read_pipe(kpipe, line, 20);
        printf("task%d read n=%d bytes from pipe : [", running->pid, n);
        for (i=0; i<n; i++)
            kputc(line[i]);
        printf("]\n");
    }
}

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

int kprintf(char *fmt, ...);

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & 0x80000000){
        if (sicstatus & 0x08){
            kbd_handler();
        }    
        if (vicstatus & (1<<12)) // bit12=1: uart0
            uart_handler(&uart[0]);
        if (vicstatus & (1<<13)) // bit13=1: uart1
            uart_handler(&uart[1]);
        if (vicstatus & (1<<31)){ // PIC.bit31= SIC interrupts
            if (sicstatus & (1<<3)){ // SIC.bit3 = KBD interrupt
                kbd_handler();
            }
        }
    }
}

int body();

int main()
{
    int i; 
    char line[128]; 
    u8 kbdstatus, key, scode;

    color = WHITE;
    row = col = 0; 

    fbuf_init();
    kprintf("Welcome to Heidi's Wanix in ARM\n");
    kbd_init();
    uart_init();
    
    pipe_init(); // initialize PIPE
    kpipe = create_pipe(); // create global kpipe
    kernel_init();

    /* enable timer0,1, uart0,1 SIC interrupts */
    VIC_INTENABLE = 0;
    VIC_INTENABLE |= (1<<12); // UART0 at bit12
    VIC_INTENABLE |= (1<<13); // UART1 at bit13
    VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
    SIC_INTENABLE = 0;
    SIC_INTENABLE |= (1<<3); // KBD int=bit3 on SIC
    SIC_ENSET |= (1<<3);
    
    kprintf("P0 kfork tasks\n");
    kfork((int)pipe_writer, 1); // pipe writer process
    kfork((int)pipe_reader, 1); // pipe reader process
    while(1){
        if (readyQueue)
            tswitch();
    }
}
