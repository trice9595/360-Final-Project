/*************** type.h file ******************/

#include <ext2fs/ext2_fs.h>


typedef struct minode{
  struct ext2_inode inode;
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct MOUNT *mptr;
  int lock;
}MINODE;

typedef struct oft{
  int  mode;
  int  refCount;
  MINODE *mptr;
  int  offset;
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          ppid;
  int          status;
  int          uid, gid;
  MINODE      *cwd;
  OFT         *fd[16]; //NFD
}PROC;

typedef struct mount{
        int    dev;
        int    nblocks,ninodes;
        int    bmap, imap, iblk;
        MINODE *mounted_inode;
        char   name[64]; 
        char   mount_name[64];
}MOUNT;
//================= end of type.h ===================

