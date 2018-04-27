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
} STAT;
**************************************************************************/


STAT utat, *sp;
int fd, n;
DIR *dp;
char f[32], cwdname[64], file[64];
char buf[1024];

DIR *dp;
char *cp;


/* List file permissions */
void ls_file(sp,name,path) struct stat *sp; char *name, *path;
{
    int mode, k, len;
    int mask = 000400;

    char fullname[32], linkname[60];

    mode = sp->st_mode;

    if ((mode & 0040000) == 0040000)
        mputc('d');

    if ((mode & 0120000) == 0120000)
        mputc('s');

    else if ((mode & 0100000) == 0100000)
         mputc('-');
    
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

    printf("%d ", sp->st_size);

    printf("%s", name);

    if ((mode & 0120000)== 0120000){
      strcpy(fullname, path); strcat(fullname, "/"); strcat(fullname,name);
      // symlink file: get its linked string
      len = readlink(fullname, linkname);
      printf(" -> %s", linkname);
    }

    printf("\n\r");     

}

/* List a given directory, and all of its files */
void ls_dir(sp, path) struct stat *sp; char *path;
{
    STAT dstat, *dsp;
    long size;
    char temp[32];

    size = sp->st_size;

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

int main(int argc, char *argv[ ]) {
    print2f("========================================\n\r");
    print2f("= --- --      Heidi's LS      - -- --- =\n\r");
    print2f("========================================\n\r");

    sp = &utat;


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

