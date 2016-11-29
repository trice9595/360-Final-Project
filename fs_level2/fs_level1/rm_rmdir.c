

int fs_rmdir(char* pathname)
{
	//get in-memory inode
	int num_entries;
	int pino;
	int ino = getino(&dev, pathname);

	if(ino == 0)
	{
		printf("File not found\n");
		return 0;
	}

	char* base = basename(pathname);

	MINODE* pmip = NULL;
	MINODE* mip = iget(dev, ino);

	//verify inode is a dir

	if(!S_ISDIR(mip->inode.i_mode))
	{
		printf("Invalid path\n");
	}


	//minode is not busy
	if(mip->refCount != 1)
	{
		printf("Minode is busy\n");
		printf("refcount = %d\n", mip->refCount);
		iput(mip);
		return 0;
	}

	//dir only contains . and .. entries
	num_entries = get_num_entries(&mip->inode);
	if(num_entries > 2)
	{
		printf("DIR is not empty. DIR has %d entries\n", num_entries);
		iput(mip);
		return 0;
	}

	//get parents ino and inode
	pino = findino(mip); 

	 //get parent from .. entry in INODE.i_block[0]
	pmip = iget(mip->dev, pino);

	 //find name from parent DIR
	findmyname(pmip, ino, base);

	//remove name from parent directory	

	if(base[strlen(base) - 1] == '\n')
	{
		base[strlen(base) - 1] = '\0';
	}

	rm_child(pmip, base);

	//deallocate its data blocks and inode
	fs_truncate(mip);

	//deallocate INODE
	idealloc(mip->dev, mip->ino);
	iput(mip);

	pmip->inode.i_links_count--;
	pmip->dirty = 1;
	iput(pmip);

	return 1;
}

