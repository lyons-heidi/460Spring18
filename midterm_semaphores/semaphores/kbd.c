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

// kbd.c file
#define N_SCAN 64
//#include "type.h" // hl added this 
#include "keymap"
/********* byte offsets; for char *base *********/
#define KCNTL    0x00 // 7- 6-    5(0=AT)  4=RxIntEn 3=TxIntEn  2   1   0
#define KSTAT    0x04 // 7- 6=TxE 5=TxBusy 4=RXFull  3=RxBusy   2   1   0
#define KDATA    0x08 // data register;
#define KCLK     0x0C // clock register;
#define KISTA    0x10 // int status register;

typedef volatile struct kbd{ // base = 0x1000 6000
  char *base;         // base address of KBD, as char *
  char buf[128];
  int head, tail;
  struct SEMAPHORE data, line; // data.value = 0, data.queue = 0
}KBD;

extern int color;
volatile KBD kbd;
int kputc(char);

int kbd_init()
{
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000; // KBD base in versatilepb
  kp->head = kp->tail = 0;

  //*(kp->base+KCNTL) = 0x14; // 0001 0100
  //*(kp->base+KCLK)  = 8;
  kp->data.value = 0; kp->data.queue = 0;
  kp->line.value = 0; kp->line.queue = 0; 
}

void kbd_handler()
{
  u8 scode, c;
  KBD *kp = &kbd; 
  scode = *(kp->base+KDATA);
  if (scode &0x80)
    return;
  
  if ( kp->data.value == 8 ) // input buffer is full
    return;

  c = unsh[scode];
  kp->buf[kp->head++] = c; // enter key from circular buff
  kp->head %= 8;

  V(&kp->data);

  if (c=='\r') // return key: has an input
    V(&kp->line);
}

int kputc(char);  // kputc() in vid.c driver

int kgetc()
{
  char c;
  KBD *kp = &kbd;
  P(&kp->data); // P on KBD's data semaphore

  lock();
    c = kp->buf[kp->tail++]; // get a c and update tail index
    kp->tail %= 8;
  unlock(); // enable IRQ interrupts

  return c;
}

int kgets(char *line)
{
  char c;
  while((c=kgetc()) != '\r'){
    *line++ = c;
  }
  *line = 0;
}


int kbd_task(){
  char line[128];
  KBD *kp = &kbd;
  while(1) {
    P(&kp->line); // wait for a line
    kgets(line);
    printf("line = %s\n", line);
  }
}