#include "fs_level1.c"

int fs_open(char* file, int flags)
{
	int ino = 0, permission = 0, i = 0;
	OFT* myTable = NULL;
	MINODE* mip = NULL;

	if(flags < 0 || flags > 3)
	{
		printf("invalid flag: %d\n", flags);
		return -1;
	}

	//1. get file's minode:
	ino = getino(&dev, file);
	if (ino == 0)
	{
		printf("can't handle create functionality\n");
		return -1;
		//creat(file);
		//ino = getino(&dev, file)
	}
	mip = iget(dev, ino);
	
	//2. check file INODE's access permission;

	permission = get_permission(mip);

	//if permission doesn't allow read
	if(!(permission & 0x0100))
		if(flags == 0 || flags == 2)
			return -1;

	//doesn't allow write
	if(!(permission & 0x0080))
		if(flags == 1 || flags == 2)
			return -1;

	//   for nonspecial file, check for incompatible open modes;	

	
	//3. allocate an openTable entry;
	myTable = malloc(sizeof(OFT));

	//   initialize openTable entries;
	myTable->mode = flags;
	myTable->refCount = 1;
	myTable->mptr = mip;

	//   set byteOffset = 0 for R|W|RW; set to file size for append mode;
	if(flags >= 0 && flags <= 2)	
		myTable->offset = 0;
	else if(flags == 3)
		myTable->offset = mip->inode.i_size;
	   
	//4. Search for a FREE fd[] entry with the lowest index fd in PROC;
	while(running->fd[i] != NULL)
		i++;
	
	//   Let fd[fd] point to the openTable entry;
	running->fd[i] = myTable;	

	//5. unlock minode;
	mip->lock = 0;

	//   return fd as the file descriptor;
	return i;
}

int fs_close(int fd)
{
	
	//1. check fd is a valid open file descriptor;
	
	if (running->fd[fd] != NULL)
	   {
		/*
		if (running->fd[fd]->mode == READ WRITE PIPE)
		{
			return close_pipe(fd); //close pipe descriptor
		}
		*/
 		//if last process using this OFT
		running->fd[fd]->refCount--;
		if (running->fd[fd]->refCount == 0)
		   {
		   		running->fd[fd]->mptr->lock = 1;
				 //release minode
				iput(running->fd[fd]->mptr);
		   }
	   }

	 running->fd[fd] = 0; //clear fd to 0
	
	return 1;
	
}

int fs_lseek(int fd, int position)
{
	//set file's OFT to beginning of file + postition
	


}

u32 map(INODE* iptr, int lbk)
{
	u32 dblk = 0;
	char ibuf[BLKSIZE];

	if(lbk < 12)//direct blocks
	{
		blk = iptr->i_block[lbk];
	}
	else if(12 <= lbk < 12+256) //indirect blocks
	{
		get_block(dev, iptr->i_block[12], ibuf);
		blk = ibuf[lbk-12];
	}
	else //double indirect blocks
	{
		get_block(dev, iptr->i_block[13], ibuf);		
		lbk -= (12+256);
		dblk = ibuf[lbk / 256];

		get_block(dev, dblk, ibuf);
		blk = ibuf[lbk %256];
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
    char *cq = fbuf, *cp = NULL; // cq points at buf[ ]

    while (nbytes && avil){

       //Compute LOGICAL BLOCK number lbk and startByte in that block from offset;

       lbk = oftp->offset / BLKSIZE;
       startByte = oftp->offset % BLKSIZE;
     
       
       blk = map(&mip->inode, lbk);

       /* get the data block into readbuf[BLKSIZE] */
       get_block(mip->dev, blk, readbuf);

       /* copy from startByte to buf[ ], at most remain bytes in this block */
       cp = readbuf + startByte;   
       remain = BLKSIZE - startByte;   // number of bytes remain in readbuf[]
       while (remain > 0){
            *cq++ = *cp++; // copy byte from readbuf[] into buf[]
             oftp->offset++; // advance offset 
             count++; // inc count as number of bytes read
             avil--; nbytes--;  remain--;
             if (nbytes <= 0 || avil <= 0) 
                 break;
       }
 
       // if one data block is not enough, loop back to OUTER while for more ...

   }
   printf("myread: read %d char from file descriptor %d\n", count, fd);  
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
		return -1;
	}
	
	mip = running->cwd;

	//2 (regular file):
	//return read_file(fd, buf, nbytes, space);
	if(!S_ISREG(mip->inode.i_mode))
	{
		return -1;
	}

	return myread(fd, fbuf, nbytes);
}

int write_file(int fd, char* fbuf, int nbytes)
{
	MINODE* mip = NULL;
	int mode = running->fd[fd]->mode;
	if(mode != 1 && mode != 2 && mode != 3)
	{
		return -1;
	}
	
	mip = running->cwd;

	//2 (regular file):
	//return read_file(fd, buf, nbytes, space);
	if(!S_ISREG(mip->inode.i_mode))
	{
		return -1;
	}

	return mywrite(fd, buf, nbytes);

}

int mywrite(int fd, char* fbuf, int nbytes)
{
	char writebuf[BLKSIZE];
	MINODE* mip = running->fd[fd]->mptr;
	OFT* oftp = running->fd[fd];
 	int lbk = 0, startByte = 0, remain = 0;
    char *cq = fbuf, *cp = NULL; // cq points at buf[ ]

    while (nbytes > 0){

       //Compute LOGICAL BLOCK number lbk and startByte in that block from offset;

       lbk = oftp->offset / BLKSIZE;
       startByte = oftp->offset % BLKSIZE;  
       
       blk = map(&mip->inode, lbk);

       /* get the data block into readbuf[BLKSIZE] */
       get_block(mip->dev, blk, writebuf);

       /* copy from startByte to buf[ ], at most remain bytes in this block */
       cp = writebuf + startByte;   
       remain = BLKSIZE - startByte;   // number of bytes remain in readbuf[]
       while (remain > 0){
            *cp++ = *cq++; // copy byte from readbuf[] into buf[]
			oftp->offset++; // advance offset 
			nbytes--;  remain--;

			if(oftp->offset > mip->inode.i_size)
				mip->inode.i_size++; //inc file size
            if (nbytes <= 0) 
                 break;
       }
 		put_block(mip->dev, blk, writebuf);
       // if one data block is not enough, loop back to OUTER while for more ...

   }
	mip->dirty = 1;
   printf("mywrite: wrote %d char into file descriptor %d\n", nbytes, fd);  
   return nbytes;   //number of bytes written
}

void fs_cat()
{

}

void fs_cp()
{

}

void fs_mv()
{

}
