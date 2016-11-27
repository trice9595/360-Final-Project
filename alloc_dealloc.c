#include "util.c"

/*********** alloc_dealloc.c file *************/

int tst_bit(char *buf, int bit)
{
  int i, j;

  i = bit / 8;
  j = bit % 8;
  if (buf[i] & (1 << j)){
    return 1;
  }
  return 0;
}

int clr_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] &= ~(1 << j);
  return 0;
}

int set_bit(char *buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  buf[i] |= (1 << j);
  return 0;
}

int incFreeInodes(int dev)
{
  // inc free inodes count in SUPER and GD
  get_block(dev, 1, buf2);
  sp = (SUPER *)buf2;
  sp->s_free_inodes_count++;
  put_block(dev, 1, buf2);

  get_block(dev, 2, buf2);
  gp = (GD *)buf2;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, buf2);
}

int decFreeInodes(int dev)
{
  // I DID IT... maybe...

	//dec free inodes count in SUPER and GD
	get_block(dev, 1, buf2);
	sp = (SUPER *)buf2;
	sp->s_free_inodes_count--;
	put_block(dev, 1, buf2);

	get_block(dev, 2, buf2);
	gp = (GD *)buf2;
	gp->bg_free_inodes_count--;
	put_block(dev, 2, buf2);
}

int incFreeBlocks(int dev)
{

  // inc free block count in SUPER and GD
  get_block(dev, 1, buf2);
  sp = (SUPER *)buf2;
  sp->s_free_blocks_count++;
  put_block(dev, 1, buf2);

  get_block(dev, 2, buf2);
  gp = (GD *)buf2;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, buf2);
}

int decFreeBlocks(int dev)
{
  // I DID IT... maybe...

	//dec free block count in SUPER and GD
	get_block(dev, 1, buf2);
	sp = (SUPER*)buf2;
	sp->s_free_blocks_count--;
	put_block(dev, 1, buf2);

	get_block(dev, 2, buf2);
	gp = (GD*)buf2;
	gp->bg_free_blocks_count--;
	put_block(dev, 2, buf2);
}


u32 ialloc(int dev)
{
 int i;

	printf("allocating inode...\n");
 // get inode Bitmap into buf
 get_block(dev, imap, buf);
 
 for (i=0; i < ninodes; i++){
   if (tst_bit(buf, i)==0){
     set_bit(buf, i);
     put_block(dev, imap, buf);

     // update free inode count in SUPER and GD
     decFreeInodes(dev);
     
     printf("ialloc: ino=%d\n", i+1);
     return (i+1);
   }
 }
 return 0;
} 

void idealloc(int dev, int ino)
{
  int i;  

  if (ino > ninodes){
    printf("inumber %d out of range\n", ino);
    return;
  }

  // get inode bitmap block
  get_block(dev, imap, buf);
  clr_bit(buf, ino-1);

  // write buf back
	
  put_block(dev, imap, buf);

  // update free inode count in SUPER and GD
  incFreeInodes(dev);
}

u32 balloc(int dev)
{
  // I DID IT... maybe...
	int i;
	printf("allocating block...\n");
 	// get block Bitmap into buf
 	get_block(dev, bmap, buf);
 
 	for (i=0; i < nblocks; i++){
	   if (tst_bit(buf, i)==0){
		 set_bit(buf, i);
		 put_block(dev, bmap, buf);

		 // update free block count in SUPER and GD
		 decFreeBlocks(dev);
		 
		 printf("balloc: block=%d\n", i+1);
		 return (i+1);
	   }
	}
}

/*
int bdealloc(int dev, int bit)
{
  // I DID IT... maybe...
	int i;  
  char buf[BLKSIZE];

  if (ino > nblocks)
  {
    printf("inumber %d out of range\n", ino);
    return;
  }

  // get block bitmap inode
  get_block(dev, imap, buf);
  clr_bit(buf, ino - 1);

  // write buf back
  put_block(dev, bmap, buf);

  // update free inode count in SUPER and GD
  incFreeBlocks(dev);
}
*/

