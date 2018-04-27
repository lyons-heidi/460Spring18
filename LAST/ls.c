/********************************************************************
Copyright 2010-2015 K.C. Wang, <kwang@eecs.wsu.edu>
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
#include "ucode.c"

#define BLK 1024
#define OWNER  000700
#define GROUP  000070
#define OTHER  000007

/******************************************************************
typedef struct stat {
  ushort st_dev;		// major/minor device number 
  ushort st_ino;		// i-node number 
  ushort st_mode;		// file mode, protection bits, etc.
  ushort st_nlink;		// # links; TEMPORARY HACK: should be nlink_t
  ushort st_uid;		// uid of the file's owner 
  ushort st_gid;		// gid; TEMPORARY HACK: should be gid_t
  ushort st_rdev;
  long   st_size;		// file size 
  long   st_atime;		// time of last access 
  ******************** KCW : order in ionde is ctime, mtime ***************
  //long   st_mtime;		// time of last data modification
  //long   st_ctime;		// time of last file status change
  ***************** revirse order to make storing time in MTX easier *********
  long   st_ctime;		// time of last data modification
  long   st_mtime;		// time of last file status change
  ****** KCW: order of ctime and mtime are reversed in inode 
} STAT;
**************************************************************************/
//int strcat(), strncpy();

STAT utat, *sp;
int fd, n;
DIR *dp;
char f[32], cwdname[64], file[64];
char buf[1024];

DIR *dp;
char *cp;

void pdate(t) u8 t[4];
{
   printf("%c%c%c%c-%c%c-%c%c  ", 
          (t[0]>>4)+'0', (t[0]&0x0F)+'0',
          (t[1]>>4)+'0', (t[1]&0x0F)+'0', 
          (t[2]>>4)+'0', (t[2]&0x0F)+'0',
          (t[3]>>4)+'0', (t[3]&0x0F)+'0');
}

void ptime(t) u8 t[4];
{
   printf("%c%c:%c%c:%c%c  ", 
   (t[0]>>4)+'0', (t[0]&0x0F)+'0',
   (t[1]>>4)+'0', (t[1]&0x0F)+'0', 
   (t[2]>>4)+'0', (t[2]&0x0F)+'0');
}

void ls_file(sp,name,path) struct stat *sp; char *name, *path;
{
    int mode, mask, k, len;
    char fullname[32], linkname[60];

    mode = sp->st_mode;

    if ((mode & 0040000) == 0040000)
        mputc('d');

    if ((mode & 0120000) == 0120000)
        mputc('s');
    else if ((mode & 0100000) == 0100000)
         mputc('-');


    mask = 000400;
    for (k=0; k<3; k++){
        if (mode & mask)
            mputc('r');
        else
            mputc('-');
        mask = mask >> 1;
        if (mode & mask)
            mputc('w');
        else
            mputc('-');
        mask = mask >> 1;
        if (mode & mask)
            mputc('x');
        else
            mputc('-');
        mask = mask >> 1;
    }

    if (sp->st_nlink < 10)
        printf("  %d ", sp->st_nlink);
    else
        printf(" %d ", sp->st_nlink);


    printf(" %d  %d", sp->st_uid, sp->st_gid);
    //align(sp->st_size);
    printf("%d ", sp->st_size);

    pdate(&sp->st_date); ptime(&sp->st_time);

    printf("%s", name);

    if ((mode & 0120000)== 0120000){
      strcpy(fullname, path); strcat(fullname, "/"); strcat(fullname,name);
      // symlink file: get its linked string
      len = readlink(fullname, linkname);
      printf(" -> %s", linkname);
    }

    printf("\n\r");     

}

void ls_dir(sp, path) struct stat *sp; char *path;
{
    STAT dstat, *dsp;
    long size;
    char temp[32];

    size = sp->st_size;

    // printf("ls_dir %s\n", path); //getc();

    fd = open(file, O_RDONLY); /* open dir file for READ */

    while ( (n = read(fd, buf, 1024))){

      cp = buf;
      dp = (DIR *)cp;

      while (cp < &buf[1024]){
        if (dp->inode != 0){
           dsp = &dstat;
           strncpy(temp, dp->name, dp->name_len); 
           temp[dp->name_len] = 0;  

           strcpy(f, file);
           strcat(f, "/"); strcat(f, temp);

           if (stat(f, dsp) >= 0){
              ls_file(dsp, temp, path);
           }
        }
        cp += dp->rec_len;
        dp = (DIR *)cp;
      }
    }

    close(fd);
}         

int main(int argc, char *argv[ ])
{
    printf("========================================\n");
    printf("= --- -- Heidi's LS in action - -- --- =\n");
    printf("========================================\n");

    sp = &utat;
    /*
    for (i=0; i<argc; i++)
      printf("arg[%d]=%s ", i, argv[i]);
    */
    //showarg(argc, argv);
    


    if (argc==1){  /* for uls without any parameter ==> cwd */
        strcpy(file, "./");
    }
    else
      strcpy(file, argv[1]);

    if (stat(file, sp) < 0){
        printf("cannot stat %s\n",argv[1]);
        exit(2);
    }

    if ((sp->st_mode & 0100000)==0100000){
        ls_file(sp, file, file);
    }
    else{
        if ((sp->st_mode & 0040000)==0040000)
	        ls_dir(sp, file);
    }

    exit(0);
}

