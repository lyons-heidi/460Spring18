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
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;


#define BLUE   0
#define GREEN  1
#define RED    2
#define WHITE  3
#define CYAN   4
#define YELLOW 5

// #include "defines.h"  # all needed types and symbols are defined above
#include "vid.c"
#include "uart.c"


extern char _binary_image1_start, _binary_volleyball_start;


int color;
/*** this is for converted images with |h|w|pixels| format ****
int show(char *p, int startRow)
{ 
   int h, w, row, col, pixel; 
   unsigned char r, g, b;

   h = (*(p+3)<<24) + (*(p+2)<<16) + (*(p+1)<<8) + *p;
   p += 4;
   w = (*(p+3)<<24) + (*(p+2)<<16) + (*(p+1)<<8) + *p;
   p += 4;          // skip over 8 byes

   uprintf("h=%d w=%d\n", h, w);
   //  if (h > 480) h = 480;
   //if (w > 640) w = 640;

   row = startRow; col = 0;
   while(1){
     r = *p; g = *(p+1); b = *(p+2);
     pixel = (b<<16) + (g<<8) + r;
     //     fb[row*640 + col] = pixel;
     fb[row*WIDTH + col] = pixel;
     p += 3;         // advance p by 3 bytes
     col++;
     if (col >= w){  // to line width of jpg image
        col = 0;
        row++;
     }
     if (row >= h+startRow)
        break;
   }
}
*******************************************/

int main()
{
   char c, *p;
   int mode;
   uart_init();
   up = upp[0];

   mode = 0;
   fbuf_init(mode);

   p = &_binary_image1_start;

   
  while(1){
    p = &_binary_image1_start;
    show_bmp(p, 0, 0); // display image1

    ufprintf(up, "enter a key from this UART : ");
    ugetc(up);
    
    fbuf_init(0);
    p = &_binary_volleyball_start;
    show_bmp(p, 0, 0);


  }

   //while(1);   // loop here  
}
