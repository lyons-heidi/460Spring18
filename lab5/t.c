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

#include <stdint.h>
#include "type.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
int BASE;
int color;

#include "uart.c"
#include "kbd.c"
#include "timer.c"
#include "vid.c"
#include "interrupts.c"
#include "queue.c"
#include "kernel.c"
#include "wait.c"
#include "fork.c"
#include "exec.c"
#include "svc.c"
#include "disk.c"
#include "loadelf.c"

int fxiahua(int r0, int r1, int r3)
{
  if (r0 < 90)
    kprintf("FXIAHUA: r0=%x r1=%x r3=%x\n", r0, r1, r3);
}
void copy_vectors(void) {
    extern uint32_t vectors_start;
    extern uint32_t vectors_end;
    uint32_t *vectors_src = &vectors_start;
    uint32_t *vectors_dst = (uint32_t *)0;

    //    kprintf("copy vectors\n");

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

// L1 entry:|31          10|9|8765|4|32|10|
//          |     addr     |0|DOM |1|00|01|
//          |              |0|0000|1|00|01|
//                          0    1     1   = addr OR 0x11        
/***************************
void xia(void) // build L1 and L2 page tables; called from reset()
{
  int i, j, pa; 
  int *t1, *t2, *t3;

  extern int L1table, L2table;

  t1 = &L1table;  // t1 -> L1table
  // zero out L1table
  for (i=0; i<4096; i++)
    t1[i] = 0;
  // creat 128 entries for 128MB RAM, ID mapped

  pa  = (int)&L2table; // pa -> L2table(s)  
  for (i=0; i<128; i++){
    t1[i] = (pa | 0x11);
    pa += 1024;      // PA inc by 1K
  }

  // create 2 entries for I/O space; ID mapped also
  //  t1 = &L1table + 256; // 256th entry of L1table
  pa = 0x10000000;     // pa=256MB beginning
  for (i=0; i<2; i++){ // 2 1MB I/O locations at t1[256-257]
    t1[256+i] = (pa | 0x11);
    pa += 1024;
  }
  // L2 tables begin at 2MB: 128K for RAM L2tables + 2 for I/O tables
  // RAM L2tables: 0x200000 - 0x220000|0x220000 for I/O L2tables
  // L2 page table entry for 4KB pages:
  // |31        12|BA987654|32|10|
  // |  address   |apAPapAP|CB|10|
  // |  address   |11111111|10|10| AP=11=RW 01=RW in Kmode, noAccess in Umode
  // |  address   |  F   F |  A  | address | 0xFFA

  t2 = &L2table;
  pa = 0;          // start with 0 PA
  /**************
  for (i=0; i<128; i++){ // 128 L2 page tables
     t3 = t2 + 256*i;
     for (j=0; j<256; j++){ // each L2 table has 256 entries; inc by 4KB 
       t3[j] = pa | 0xFFA; 
       pa + 4096;
     }
  }
  // 2 I/O L2 page tables follow
  pa = 0x10000000; // start PA=0x10000000, ID mapped
  for (i=0; i<2; i++){
    t3 = &L2table + 128*256;
    for (j=0; j<256; j++){
      t3[j] = pa | 0xFFA;
      pa += 4096;
    }
  }
  *******************/
/***         
 for (i=0; i<130; i++){ // 128 L2 page tables for RAM + 2 for I/O space
     t3 = t2 + 256*i;
     if (i==128)
       pa = 0x10000000;
     for (j=0; j<256; j++){ // each L2 table has 256 entries; inc by 4KB 
       t3[j] = pa | 0xFFA; 
       pa + 4096;
     }
  }
}
********/

int kprintf(char *fmt, ...);
void timer0_handler();

void data_handler()
{
  printf("data exception\n");
}

// IRQ interrupts handler entry point
//void __attribute__((interrupt)) kc_handler()

void irq_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);

    if (vicstatus & 0x0010){
         timer0_handler();
	 // kprintf("TIMER "); // verify timer handler return to here
    }
    if (vicstatus & 0x1000){
         uart0_handler();
	 // kprintf("U0 "); 
    }
    if (vicstatus & 0x2000){
         uart1_handler();
    }
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

extern char _binary_ramdisk_start, _binary_ramdisk_end;

int main()
{ 
   int i,a; 
   char string[32]; 
   char line[128]; 
   int size = sizeof(int);
   unsigned char kbdstatus;
   unsigned char key, scode;
   char *cp, *cq;
   int dsize;

   cp = disk = &_binary_ramdisk_start;
   dsize = &_binary_ramdisk_end - &_binary_ramdisk_start;

   // before LCD init, can't print anything yet
   cq = (char *)0x400000;
   for (i=0; i<dsize; i++){
     *cq++ = *cp++;
   }

   disk = (char *)0x400000;

   color = RED;
   row = col = 0; 
   BASE = 10;
      
   fbuf_init();
   kprintf("                     Welcome to WANIX in Arm\n");
   kprintf("LCD display initialized : fbuf = %x\n", fb);
   color = CYAN;

   kbd_init();
   uart_init();
   up = upp[0];

   shead=stail=0; sdata=0; sroom=128;
   sline = 0;

   /* enable UART0 IRQ */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at 4 
   VIC_INTENABLE |= (1<<12); // UART0 at 12
   VIC_INTENABLE |= (1<<13); // UART1 at 13
   VIC_INTENABLE = 1<<31;    // SIC to VIC's IRQ31

   //VIC_INTENABLE |= (1<<5);  // timer3,4 at 5 

   /* enable UART0 RXIM interrupt */
   UART0_IMSC = 1<<4;
      
   /* enable UART1 RXIM interrupt */
   UART1_IMSC = 1<<4;
  
   /* enable KBD IRQ */
   SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC
   kbd->control = 1<<4;


   timer_init();
   timer_start(0);
   
 ISAN:
   init();
   kprintf("RAMdisk start=%x size=%x\n", disk, dsize);
   kfork("/bin/u1");

   kprintf("P0 switch to P1 : enter a line : \n");
   kgetline(string);
 
   tswitch();  // switch to run P1 ==> never return again
}
