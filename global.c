#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;

int nblocks;
int ninodes;
int bmap;
int dev;
int imap;
int inode_start;
char buf[256];
int ubuf[256];
int ubuf2[256];

#define FREE          0
#define READY         1

#define BLKSIZE     1024
#define BLOCK_SIZE  1024

#define NMINODE      100
#define NFD           16
#define GIT_TEST       1
#define NPROC          4
