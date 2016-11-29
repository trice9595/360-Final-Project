


u32 read_map(MINODE* mptr, int lbk)
{
	int i = 0;
	char zeroes[BLKSIZE] = {0};
	u32 dblk = 0;
	u32 ibuf[256];
	u32 dbuf[256];

	if(lbk < 12)//direct blocks
	{
		blk = mptr->inode.i_block[lbk];
	}
	else if((12 <= lbk) && lbk < 12+256) //indirect blocks
	{	
		get_u32_block(dev, mptr->inode.i_block[12], ibuf);
		blk = ibuf[lbk-12];
	}
	else //double indirect blocks
	{
		get_u32_block(dev, mptr->inode.i_block[13], dbuf);

		lbk -= (12+256);
		dblk = dbuf[lbk / 256];

		get_u32_block(dev, dblk, dbuf);
		blk = dbuf[lbk % 256];
	}

	return blk;
}

u32 write_map(MINODE* mptr, int lbk)
{
	int i = 0;
	char zeroes[BLKSIZE] = {0};
	u32 dblk = 0;
	u32 ibuf[256];
	u32 dbuf[256];

	if(lbk < 12)//direct blocks
	{
		if(mptr->inode.i_block[lbk] == 0)
		{
			mptr->inode.i_block[lbk] = balloc(mptr->dev);
			put_block(mptr->dev, mptr->inode.i_block[lbk], zeroes);
		}
		blk = mptr->inode.i_block[lbk];
	}
	else if(12 <= lbk && lbk < 12+256) //indirect blocks
	{	
		//fix this shit man		
		if(mptr->inode.i_block[12] == 0)
		{
			mptr->inode.i_block[12] = balloc(mptr->dev);
			put_block(mptr->dev, mptr->inode.i_block[12], zeroes);
		}
		get_u32_block(dev, mptr->inode.i_block[12], ibuf);
		if(ibuf[lbk-12] == 0)
		{
			ibuf[lbk-12] = balloc(mptr->dev);
			put_block(mptr->dev, mptr->inode.i_block[12], zeroes);
		}
		blk = ibuf[lbk-12];
	}
	else //double indirect blocks
	{
		
		//if double indirect block is empty, allocate block
		if(mptr->inode.i_block[13] == 0)
		{
			mptr->inode.i_block[13] = balloc(mptr->dev);
			put_block(mptr->dev, mptr->inode.i_block[13], zeroes);
		}
		
		//get double indirect block
		get_u32_block(dev, mptr->inode.i_block[13], ibuf);
		lbk -= (12+256);

		//if indirect block is empty, allocate block
		if(dbuf[lbk / 256] == 0)
		{
			dbuf[lbk / 256] = balloc(mptr->dev);
			put_block(mptr->dev, dbuf[lbk / 256], zeroes);
		}

		//get indirect block
		dblk = dbuf[lbk / 256];
		get_u32_block(dev, dblk, dbuf);

		if(dbuf[lbk % 256] == 0)
		{
			dbuf[lbk % 256] = balloc(mptr->dev);
			put_block(mptr->dev, dbuf[lbk % 256], zeroes);
		}

		blk = dbuf[lbk % 256];
	}

	return blk;
}

int myread(int fd, char *fbuf, int nbytes)
{
	char readbuf[BLKSIZE];
	MINODE* mip = running->fd[fd]->mptr;
	OFT* oftp = running->fd[fd];
 	int count = 0, lbk = 0, startByte = 0, remain = 0;
    int avil = mip->inode.i_size - oftp->offset; // number of bytes still available in file.
    char *cq = fbuf, *cp = NULL; //cq points at buf[ ]

    while (nbytes && avil){
       //Compute LOGICAL BLOCK number lbk and startByte in that block from offset;
       lbk = oftp->offset / BLKSIZE;
       startByte = oftp->offset % BLKSIZE;

       blk = read_map(mip, lbk);
		printf("blk: %d\n");
       /* get the data block into readbuf[BLKSIZE] */
       get_block(mip->dev, blk, readbuf);

       /* copy from startByte to buf[ ], at most remain bytes in this block */
       cp = readbuf + startByte;   
       remain = BLKSIZE - startByte; // number of bytes remain in readbuf[]
       while (remain > 0){
            *cq++ = *cp++; // copy byte from readbuf[] into buf[]
             oftp->offset++; // advance offset 
             count++; // inc count as number of bytes read
             avil--; nbytes--;  remain--;
             if (nbytes <= 0 || avil <= 0) 
                 break;
       }
 
       // if one data block is not enough, loop back to OUTER while for more ...
   printf("myread: read %d char into file descriptor %d\n", count, fd); 
   }
	mip->dirty = 1;
   return count;   // count is the actual number of bytes read
}

//space=K|U
int read_file(int fd, char fbuf[], int nbytes)
{
	//1. validate fd; ensure oft is opened for READ or RW;
	MINODE* mip = NULL;
	int mode = running->fd[fd]->mode;
	if(mode != 0 && mode != 2)
	{	
		printf("invalid mode\n");
		return -1;
	}
	
	mip = running->fd[fd]->mptr;
	printf("mip->ino: %d\n", mip->ino);
	//2 (regular file):
	//return read_file(fd, buf, nbytes, space);

	if(!S_ISREG(mip->inode.i_mode))
	{
		printf("not reg file\n");
		return -1;
	}

	return myread(fd, fbuf, nbytes);
}

int mywrite(int fd, char* fbuf, int nbytes)
{
	char writebuf[BLKSIZE];
	MINODE* mip = running->fd[fd]->mptr;
	OFT* oftp = running->fd[fd];
 	int lbk = 0, startByte = 0, remain = 0;
    char *cq = fbuf, *cp = NULL; // cq points at buf[ ]
	int count = 0;
	printf("here\n");
    while (nbytes > 0){

       //Compute LOGICAL BLOCK number lbk and startByte in that block from offset;


       lbk = oftp->offset / BLKSIZE;
       startByte = oftp->offset % BLKSIZE;  
       
       blk = write_map(mip, lbk);
		printf("blk: %d\n");
       /* get the data block into readbuf[BLKSIZE] */
       get_block(mip->dev, blk, writebuf);

       /* copy from startByte to buf[ ], at most remain bytes in this block */

       cp = writebuf + startByte;   
       remain = BLKSIZE - startByte;   // number of bytes remain in readbuf[]
       while (remain > 0){
            *cp++ = *cq++; // copy byte from readbuf[] into buf[]		
			oftp->offset++; // advance offset 
			nbytes--;  remain--;
			
			count++;
			if(oftp->offset > mip->inode.i_size)
				mip->inode.i_size++; //inc file size
            if (nbytes <= 0) 
                 break;
       }
		
 		put_block(mip->dev, blk, writebuf);
       // if one data block is not enough, loop back to OUTER while for more ...

   }
	mip->dirty = 1;
   printf("mywrite: wrote %d char into file descriptor %d\n", count, fd);  


   return nbytes;   //number of bytes written
}

int write_file(int fd, char* fbuf, int nbytes)
{
	MINODE* mip = NULL;
	if(running->fd[fd] == NULL)
	{
		printf("INVALID FILE DESCRIPTOR\n");
		return -1;
	}
	int mode = running->fd[fd]->mode;

	if(mode != 1 && mode != 2 && mode != 3)
	{
		return -1;
	}
	
	mip = running->fd[fd]->mptr;

	//2 (regular file):
	//return read_file(fd, buf, nbytes, space);
	if(!S_ISREG(mip->inode.i_mode))
	{
		printf("Not reg file, mode is %d\n, inode is %d\n", mip->inode.i_mode);
		return -1;
	}

	return mywrite(fd, fbuf, nbytes);

}

