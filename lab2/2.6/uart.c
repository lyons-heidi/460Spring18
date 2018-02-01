/******** uart.c file of C2.5 : UART Driver Code ********/

#define TXFE 0x80
#define RXFF 0x40
#define TXFF 0x20
#define RXFE 0x10
#define BUSY 0x08
#define u32 unsigned int

/*** bytes offsets of UART registers from char *base ***/
#define UDR 0x00
#define UFR 0x18

char *tab = "0123456789ABCDEF";

typedef volatile struct uart{
    char *base;  // base address; as char *
    int n;       // uart number 0-3
}UART;

UART uart[4]; // 4 UART structures

int uart_init() // UART initialization function
{
    int i; UART *up;
    for (i=0; i<4; i++){ // uart0 to uart2 are adjacent
        up = &uart[i];
        up->base = (char *)(0x101F1000 + i*0x1000);
        up->n = i;
    }
    uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up) // input a char from UART pointed by up
{
    while (*(up->base+UFR) & RXFE); // loop if UFR is REFE
    return *(up->base+UDR); // return a char in UDR
}

int uputc(UART *up, char c) // output a char to UART pointed by up
{
    while (*(up->base+UFR) & TXFF); // loop if UFR is TXFF
    *(up->base+UDR) = c; // write char to data register
}

int upgets(UART *up, char *s) // input a string of chars
{
    while ((*s = ugetc(up)) != '\r') {
        uputc(up, *s);
        s++;
    }
    *s = 0;
}

int uprints(UART *up, char *s) // output a string of chars
{
    while (*s)
    uputc(up, *s++);
}


int urpu(UART *up, int x)
{
  char c;
  if (x){
     c = tab[x % 10];
     urpu(up, x / 10);
  }
  uputc(up, c);
}

int uprintu(UART *up, int x)
{
  if (x==0)
    uputc(up, '0');
  else
    urpu(up, x);
  uputc(up, ' ');
}

int uprinti(UART *up, int x)
{
  if (x<0){
    uputc(up, '-');
    x = -x;
  }
  uprintu(up, x);
}

int uprintf(UART *up, char *fmt, ...)   // most C compilers require the 3 dots
{
    char *cp = fmt;           // cp points to the fmt string
    int *ip = (int *)&fmt +1; // ip points to first item in stack
    while (*cp){              // scan the format string
        if (*cp != '%'){      // spit out ordinary chars
            uputc(up, *cp);
            if (*cp=='\n')    // for each '\n'
                uputc(up, '\r');   // print a '\r'
            cp++;
            continue;
        }
        cp++; // cp points at a conversion symbol
        switch(*cp){ // print item by %FORMAT symbol
            case 'c' : uputc(up, (char )*ip); break;
            case 's' : uprints(up, (char *)*ip); break;
            //case 'u' : printu((u32 )*ip); break;
            case 'd' : uprinti(up, *ip); break;
            //case 'x' : printx((u32 )*ip); break;
        }
    cp++; ip++; // advance pointers
    }
}