
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


	free(running->fd[fd]); 
	running->fd[fd] = 0;//clear fd to 0

	return 1;
	
}
