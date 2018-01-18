/*********  t.c file *********************/

int prints(char *s)
{
  while(*s)
  {
    putc(*s++);
  }
}

int gets(char s[ ])
{
    // call getc() to input a string into s[ ]
    int i = 0;
    while((s[i] = getc()) != '\r')
    {
      putc(s[i]);
      i++;
    }
    s[i] = 0;
}


char ans[64];

main()
{
  while(1){
    prints("What's your name? ");
    gets(ans);  prints("\n\r");

    if (ans[0]==0){
      prints("return to assembly and hang\n\r");
      return;
    }
    prints("Welcome "); prints(ans); prints("\n\r");
  }
}