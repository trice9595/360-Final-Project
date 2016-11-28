#include "fs_level1.c"

int fs_open(char* file, char* flags)
{
	/*
	1. get file's minode:
	   ino = getino(&dev, file);
	   if (ino==0 && O_CREAT)
	   {
		creat(file);
		ino = getino(&dev, file)
	   }
	   mip = iget(dev, ino);
	
	2. check file INODE's access permission;
	   for nonspecial file, check for incompatible open modes;
	
	3. allocate an openTable entry;
	   initialize openTable entries;
	   set byteOffset = 0 for R|W|RW; set to file size for append mode;
	   
	4. Search for a FREE fd[] entry with the lowest index fd in PROC;
	   Let fd[fd] point to the openTable entry;

	5. unlock minode;
	   return fd as the file descriptor;
	*/
}

int fs_close(int fd)
{
	/*
	1. check fd is a valid open file descriptor;
	
	2. if (PROC's fd[fd] != 0)
	   {
		3. if (openTable's mode == READ/WRITE PIPE)
		   {
			return close_pipe(fd); //close pipe descriptor
		   }

		4. if (--refCount == 0) //if last process using this OFT
		   {
			lock(minodeptr);
			iput(minode); //release minode
		   }
	   }

	5. clear fd[fd] = 0; //clear fd to 0
	
	6. return SUCCESS;
	*/
}

int fs_lseek(int fd, int position)
{
	//set file's OFT to beginning of file + postition
}

void fs_read(int fd, char* buf, int nbytes, int space)
{
	/*for regular files
	1. lock minode

	2. count = 0; //number of bytes read
	   compute bytes available in file: avil = fileSize - offset

	3. while (nbytes)
	   {
		compute logical block: lbk = offset / BLKSIZE;
		start byte in block: start = offset % BLKSIZE;

		4. convert logical block number, lbk, to physical block number, blk, through INODE.i-block[] array;

		5. read_block(dev, blk, kbuf); //read blk into kbuf[BLKSIZE]
		   char* cp = kbuf + start;
		   remain = BLKSIZE - start;

		6. while (remain)
		   {//copy bytes from kbuf[] to buf[]
			(space)? put_ubyte (*cp++, *buf++) : *buf++ = *cp++;
			offset++; count++; //inc offset, count;
			remain--; avil--; nbytes--; //dec remain, avil, nbytes;
			if (nbytes == 0 || avail == 0)
				break;
		   }
	   }

	7. unlock minode;

	8. return count;
	*/
}

void fs_write()
{
/*for regular files
	1. lock minode;

	2. count = 0; //number of bytes written

	3. while (nbytes)
	   {
		compute logical block: lbk = oftp->offset / BLOCK_SIZE;
		compute start byte: start = oftp->offset % BLOCKSIZE;

		4. convert lbk to physical block number, blk;

		5. read_block(dev, blk, kbuf); //read blk into kbuf[BLKSIZE];
		   char* cp = kbuf + start; remain = BLKKSIZE - start;

		6. while (remain)
		   {//copy bytes from kbuf[] to ubuf[]
			put_ubyte(*cp++, *ubuf++);
			offset++; count++; //inc offset, count;
			remain--; nbytes--; //dec remain, nbytes;
			if (offset > fileSize) fileSize++; //inc file size
			if (nbytes <= 0) break;
		   }

		7. write_block (dev, blk, kbuf);
	   }

	8. set minode dirty = 1; //mark minode dirty for iput()

	   unlock (minode);
	   return count;
*/
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
