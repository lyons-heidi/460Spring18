/*******************************************************
*                      t.c file                        *
*******************************************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD    *gp;
INODE *ip;
DIR   *dp;
u32 * up;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;
  
// print name of inode, and return ino number
u16 search(INODE *ip, char *name)
{
   u16 i; char c;
   u16 inum = 0;
   DIR  *dp;

   for (i=0; i<12; i++) // loop thru direct blocks
   {
       if ((u16)ip->i_block[i])
       {
          getblk((u16)ip->i_block[i], buf2);
          dp = (DIR *)buf2;

          while ((char *)dp < &buf2[1024])
          {
              c = dp->name[dp->name_len];  // save last byte
              dp->name[dp->name_len] = 0;
	            prints(dp->name); putc(' ');

              if (strcmp(dp->name, name) == 0)
              {
                inum = (u16)dp->inode;
                // prints("\n\r");
                // return inum;
              }
              dp->name[dp->name_len] = c; // restore last byte
              dp = (char *)dp + dp->rec_len;
	        }
        }
    }

    if(inum == 0)
    {
        error();
    }

    prints("\n\r");
    return inum;
}


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

int getblk(u16 blk, char *buf)
{
  readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}


main()
{ 
  u16 i, iblk, ino;
  char c;
  char * filename[64];
  char * name[2];
  name[0] = "boot", name[1] = "mtx";

  //prints("boot\n");
  getblk(2, buf1);
  gp = (GD *)buf1; // case buffer contents as type GD
  
  // 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  iblk = (u16)gp->bg_inode_table;

  // 2. WRITE YOUR CODE to get root inode
  getblk(iblk, buf1);
  ip = (INODE *)buf1 + 1; // ip is the root inode

 
  // 3. WRITE YOUR CODE to step through the data block of root inode
  // search for system name
  for (i=0; i < 2; i++)
  { 
    ino = search(ip, name[i]) - 1;

    // read block inode of ino
    getblk(iblk+(ino/8), buf1); 
    ip = (INODE *)buf1 + (ino % 8); // cast buf1 to Inode struct, set inode ptr
  }

  // read indirect block into buf2 (if it has indirect blocks)
  if ((u16)ip->i_block[12]){
      getblk((u16)ip->i_block[12], buf2);
  }

  // load the blocks into memory starting from (segment) 0x1000
  setes(0x1000);

  // direct blocks
  for (i = 0; i < 12; i++)
  {
    getblk((u16)ip->i_block[0], 0); putc('*');
    inces();
  }

  // indirect blocks
  if ((u16)ip->i_block[12])
  {
    up = (u32 *)buf2;
    while(*up)
    {
      getblk((u16)*up, 0);
      putc('.');
      inces();
      up++;
    }
  }
  prints("go?");
  getc();
}
