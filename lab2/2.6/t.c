/************** t.c file of program C2.6 ******************/
// #include "defines.h" // device base addresses, etc.
#include "vid.c" // LCD driver
#include "uart.c" // UART driver
extern char _binary_image1_start, _binary_image2_start;
#define WIDTH 640

int show_bmp(char *p, int start_row, int start_col)
{
    int h, w, pixel, rsize, i, j;
    UART * up = &uart[0]; // adding in uart for uprintf statement
    unsigned char r, g, b;
    char *pp;
    int *q = (int *)(p+14); // skip over 14-byte file header
    w = *(q+1); // image width in pixels
    h = *(q+2); // image height in pixels
    p += 54; // p-> pixels in image
    
    //BMP images are upside down, each row is a multiple of 4 bytes
    rsize = 4*((3*w + 3)/4); // multiple of 4
    p += (h-1)*rsize; // last row of pixels
    for (i=start_row; i<(start_row + h); i++){
        pp = p;
        for (j=start_col; j<(start_col + w); j++){
            if (((i + j)%2) == 0)
            {
                b = *pp; g = *(pp+1); r = *(pp+2); // BRG values
                pixel = (b<<16) | (g<<8) | r; // pixel value
                fb[(i/2)*WIDTH + (j/2)] = pixel; // write to frame buffer 
            }
            pp+=3; // advance pp to next pixel (rows)
        }
        p -= rsize; // to preceding row (cols)
    }
    uprintf(up, "\nBMP image height=%d width=%d\n", h, w);
}

// int show_bmp(char *p, int startRow, int startCol)
// { 
//    int h, w, pixel, r1, r2, i, j; 
//    unsigned char r, g, b;
//    char *pp;
//    UART *up = &uart[0]; // this is for print statement
//    int *q = (int *)(p+14); // skip over 14 bytes file header 
//    q++;                    // skip 4 bytes in image header
//    w = *q;                 // width in pixels 
//    h = *(q + 1);           // height in pixels

//    p += 54;                // p point at pixels now 

//    // but the picture is up-side DOWN

//    r1 = 3*w;
//    r2 = 4*((3*w+3)/4);     // row size is a multiple of 4 bytes  

//    p += (h-1)*r2;
//    for (i=startRow; i<h+startRow; i++){
//      pp = p;
//      for (j=startCol; j<startCol+w; j++){
//          b = *pp; g = *(pp+1); r = *(pp+2);
//          pixel = (b<<16) + (g<<8) + r;
//          fb[i*640 + j] = pixel;
//          pp += 3;    // back pp by 3 bytes
//      }
//      p -= r2;
//    }
//    uprintf(up, "\nBMP image height=%d width=%d\n", h, w);
// }


int main()
{
    char c,* p;
    uart_init(); // initialize UARTs
    UART * up = &uart[0]; // use UART0
    fbuf_init(1); // default to VGA mode

    while(1){
        p = &_binary_image1_start;
        show_bmp(p, 0, 0); // display image1
        uprintf(up, "enter a key from this UART : ");
        ugetc(up);
        // p = &_binary_image2_start;
        // show_bmp(p,120, 0); // display image2
    }
    while(1); // loop here
}