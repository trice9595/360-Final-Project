


int add_dir_entry(int ino, char* basename, int rec_len, int file_type)
{
	strcpy(dp->name, basename);
	dp->name_len = strlen(basename);
	dp->rec_len = rec_len;
	dp->file_type = file_type;
	dp->inode = ino;
	print_dir();
}

int get_last_entry(char* cp)
{
   while (cp < buf + BLKSIZE)
	{
   		dp = (DIR *)cp;
        print_dir();
		if(dp->rec_len <= 0)
		{
			printf("ERROR: dp->rec_len of %d is invalid\n", dp->rec_len);
			break;
		}
        cp += dp->rec_len;


   }
}

void enter_child(MINODE* pmip, int ino, char* basename, int file_type)
{
	int i, need_len = 0, remain = 0, ideal_len = 0;
	char *cp;
	char exBuf[10];


	need_len = 4*((8+strlen(basename)+4)/4);
	
	printf("entering child...\n");
   for (i=0; i<12; i++)
	{  // ASSUME DIRs only has 12 direct blocks
       if (pmip->inode.i_block[i] == 0)
		{
			return;
		}

       get_block(dev, pmip->inode.i_block[i], buf);
		
        cp = buf;
		dp = (DIR *)cp;

		if(dp->inode == 0)
		{
			printf("*******\n********\n*******\n");
			printf("*******\nfirst\n*******\n");
			printf("*******\nentry\n*******\n");
			printf("*******\ninsert\n*******\n");
			printf("*******\n********\n*******\n");
			add_dir_entry(ino, basename, BLKSIZE, file_type);
			return;
		}
		
	    print_dir();
		cp += dp->rec_len;

		get_last_entry(cp);

		ideal_len = 4*((8 + dp->name_len + 4)/4);
		remain = dp->rec_len - ideal_len;

		if(remain >= need_len)
		{
			dp->rec_len = ideal_len;
			dp = (DIR *)((char *)dp + dp->rec_len);
			add_dir_entry(ino, basename, remain, file_type);
		}
		else
		{
			//allocate new data block?????
			printf("*******\nallocate\n*******\n");
			printf("*******\nnew\n*******\n");
			printf("*******\ndata\n*******\n");
			printf("*******\nblock\n*******\n");

		}

		put_block(dev, pmip->inode.i_block[i], buf);
	}
	
}


void kmkdir(MINODE* pmip, char* basename)
{
	char* cp;
	int i, ino = ialloc(dev);
	int blk = balloc(dev);
	MINODE* mip = iget(dev, ino);
	

	mip->inode.i_block[0] = blk;
	for(i = 1; i < 12; i++)
	{
		mip->inode.i_block[i] = 0;
	}
	mip->inode.i_mode = 0x41A4;
	mip->dirty = 1;
	iput(mip);

	//make data block 0 of inode to contain . and .. entries
	ip = &mip->inode;

	get_block(dev, ip->i_block[0], buf);


	cp = buf;

	dp = (DIR *)cp;
	strcpy(dp->name, ".");
	dp->name_len = 1;
	dp->file_type = 2;
	dp->rec_len = 12;
	dp->inode = ino;


    cp += dp->rec_len;
	dp = (DIR *)cp;
    
	strcpy(dp->name, "..");
	dp->name_len = 2;
	dp->file_type = 2;
	dp->rec_len = 1012;
	dp->inode = pmip->ino;

	//write to disk block blk
	put_block(dev, blk, buf);



	enter_child(pmip, ino, basename, 2);
	

    //which enters (ino, basename) as a DIR entry to the parent INODE

}

void mkdir_fs(char* pathname)
{
	MINODE* pmip = NULL;
	char* base = basename(pathname);
	char* dir = dirname(pathname);
	int i = 0, myino, pino;

	base[strlen(base) - 1] = '\0';

	if(base == NULL || strcmp(base, ".") == 0)
		return;

	if(strcmp(dir, ".") != 0)
	{
		printf("dir: %s\n", dir);
		pino = getino(&dev, dir);
	}
	else
	{
		pino = running->cwd->ino;
	}
	pmip = iget(dev, pino);
	
	if(!S_ISDIR(pmip->inode.i_mode))
	{
		printf("Invalid path with i_mode #%d\n", pmip->inode.i_mode);
		ip = &pmip->inode;
		print_inode();
		print_inode_contents();
		return;
	}
	
	if(search_inode(&pmip->inode, base) != 0)
	{
		printf("Directory already exists\n");
		return;
	}

	kmkdir(pmip, base);

	pmip->inode.i_links_count++;
	pmip->dirty = 1;
	iput(pmip);


}


void kcreat(MINODE* pmip, char* basename)
{
	char* cp;
	int i, ino = ialloc(dev);
	//int blk = balloc(dev);
	MINODE* mip = iget(dev, ino);
	
	//do I need to mark it dirty and put it

	mip->dirty = 1;
	iput(mip);

	//make data block 0 of inode to contain . and .. entries
	ip = &mip->inode;

	//test if done correctly
	ip->i_mode = 0x81A4;

	enter_child(pmip, ino, basename, 1);
	

    //which enters (ino, basename) as a DIR entry to the parent INODE

}

void creat_fs(char* pathname)
{
	MINODE* pmip = NULL;
	char* base = basename(pathname);
	char* dir = dirname(pathname);

	base[strlen(base) - 1] = '\0';
	int i = 0, myino, pino;


	if(strcmp(base, ".") == 0 || 
		strcmp(base, "/") == 0)
		return;

	printf("getting pino with dir: %s\n", dir);
	pino = getino(&dev, dir);
	printf("got pino #%d\n", pino);
	
	pmip = iget(dev, pino);
	
	if(!S_ISDIR(pmip->inode.i_mode))
	{
		printf("Invalid path with i_mode #%d\n", pmip->inode.i_mode);
		ip = &pmip->inode;
		return;
	}
	
	if(search_inode(&pmip->inode, base) != 0)
	{
		printf("File already exists\n");
		return;
	}

	kcreat(pmip, base);

	pmip->inode.i_links_count++;
	pmip->dirty = 1;
	iput(pmip);
}

