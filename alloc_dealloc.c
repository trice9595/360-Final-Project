#include "util.c"

/*********** alloc_dealloc.c file *************/

int tst_bit(char *temp_buf, int bit)
{
  int i, j;

  i = bit / 8;
  j = bit % 8;
  if (temp_buf[i] & (1 << j)){
    return 1;
  }
  return 0;
}

int clr_bit(char *temp_buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  temp_buf[i] &= ~(1 << j);
  return 0;
}

int set_bit(char *temp_buf, int bit)
{
  int i, j;
  i = bit / 8;
  j = bit % 8;
  temp_buf[i] |= (1 << j);
  return 0;
}

int incFreeInodes(int dev)
{
	 char temp_buf[BLKSIZE];
  // inc free inodes count in SUPER and GD
  get_block(dev, 1, temp_buf);
  sp = (SUPER *)temp_buf;
  sp->s_free_inodes_count++;
  put_block(dev, 1, temp_buf);

  get_block(dev, 2, temp_buf);
  gp = (GD *)temp_buf;
  gp->bg_free_inodes_count++;
  put_block(dev, 2, temp_buf);
}

int decFreeInodes(int dev)
{
  // I DID IT... maybe...
   char temp_buf[BLKSIZE];

	//dec free inodes count in SUPER and GD
	get_block(dev, 1, temp_buf);
	sp = (SUPER *)temp_buf;
	sp->s_free_inodes_count--;
	put_block(dev, 1, temp_buf);

	get_block(dev, 2, temp_buf);
	gp = (GD *)temp_buf;
	gp->bg_free_inodes_count--;
	put_block(dev, 2, temp_buf);
}

int incFreeBlocks(int dev)
{
  char temp_buf[BLKSIZE];

  // inc free block count in SUPER and GD
  get_block(dev, 1, temp_buf);
  sp = (SUPER *)temp_buf;
  sp->s_free_blocks_count++;
  put_block(dev, 1, temp_buf);

  get_block(dev, 2, temp_buf);
  gp = (GD *)temp_buf;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, temp_buf);
}

int decFreeBlocks(int dev)
{
  // I DID IT... maybe...
 	char temp_buf[BLKSIZE];

	//dec free block count in SUPER and GD
	get_block(dev, 1, temp_buf);
	sp = (SUPER*)temp_buf;
	sp->s_free_blocks_count--;
	put_block(dev, 1, temp_buf);

	get_block(dev, 2, temp_buf);
	gp = (GD*)temp_buf;
	gp->bg_free_blocks_count--;
	put_block(dev, 2, temp_buf);
}


u32 ialloc(int dev)
{
 int i;
 char temp_buf[BLKSIZE];

	printf("allocating inode...\n");
 // get inode Bitmap into temp_buf
 get_block(dev, imap, temp_buf);
 
 for (i=0; i < ninodes; i++){
   if (tst_bit(temp_buf, i)==0){
     set_bit(temp_buf, i);
     put_block(dev, imap, temp_buf);

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
  char temp_buf[BLKSIZE];

  if (ino > ninodes){
    printf("inumber %d out of range\n", ino);
    return;
  }

  // get inode bitmap block
  get_block(dev, imap, temp_buf);
  clr_bit(temp_buf, ino-1);

  // write temp_buf back
	
  put_block(dev, imap, temp_buf);

  // update free inode count in SUPER and GD
  incFreeInodes(dev);
}

u32 balloc(int dev)
{
  // I DID IT... maybe...
	int i;
 	char temp_buf[BLKSIZE];
	printf("allocating block...\n");
 	// get block Bitmap into temp_buf
 	get_block(dev, bmap, temp_buf);
 
 	for (i=0; i < nblocks; i++){
	   if (tst_bit(temp_buf, i)==0){
		 set_bit(temp_buf, i);
		 put_block(dev, bmap, temp_buf);

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
  char temp_buf[BLKSIZE];

  if (ino > nblocks)
	{
    printf("inumber %d out of range\n", ino);
    return;
  }

  // get block bitmap inode
  get_block(dev, imap, temp_buf);
  clr_bit(temp_buf, ino - 1);

  // write temp_buf back
  put_block(dev, bmap, temp_buf);

  // update free inode count in SUPER and GD
  incFreeBlocks(dev);
}
*/
