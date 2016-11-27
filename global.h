#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>


#define FREE          0
#define READY         1

#define BLKSIZE     1024
#define BLOCK_SIZE  1024

#define NMINODE      100
#define NFD           16
#define NPROC          4

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

extern SUPER *sp;
extern GD    *gp;
extern INODE *ip;
extern DIR   *dp;

extern MINODE minode[NMINODE];
extern PROC   proc[NPROC], *running;
extern MINODE *root;

extern int dev, imap, bmap;
extern int ninodes, nblocks;
extern int blk, offset;
extern int inodes_begin_block, inodes_per_block;
extern char buf[BLKSIZE];
extern char buf2[BLKSIZE];
