
int add_dir_entry(int ino, char* basename, int rec_len)
{
	strcpy(dp->name, basename);
	dp->name_len = strlen(basename);
	dp->rec_len = rec_len;
	dp->file_type = 2;
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

void enter_child(MINODE* pmip, int ino, char* basename)
{
	int i, need_len = 0, remain = 0, ideal_len = 0;
	char *cp;
	char exBuf[10];

	need_len = 4*((8+strlen(basename)+3)/4);

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
			add_dir_entry(ino, basename, BLKSIZE);
			return;
		}
		
	    print_dir();
		cp += dp->rec_len;

		get_last_entry(cp);

		ideal_len = 4*((8 + dp->name_len + 3)/4);
		remain = dp->rec_len - ideal_len;

		if(remain >= need_len)
		{
			dp->rec_len = ideal_len;
			dp = (DIR *)((char *)dp + dp->rec_len);
			add_dir_entry(ino, basename, remain);
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

	mip->dirty = 1;
	iput(mip);

	//make data block 0 of inode to contain . and .. entries
	ip = &mip->inode;

	get_block(dev, ip->i_block[0], buf);

	dp = (DIR *)buf;
	strcpy(dp->name, ".");
	dp->name_len = 1;
	dp->file_type = 2;
	dp->rec_len = 12;
	dp->inode = ino;


    dp = dp + dp->rec_len;
    
	strcpy(dp->name, "..");
	dp->name_len = 2;
	dp->file_type = 2;
	dp->rec_len = 1012;
	dp->inode = pmip->ino;

	//write to disk block blk
	put_block(dev, blk, buf);
   
	enter_child(pmip, ino, basename);
	

    //which enters (ino, basename) as a DIR entry to the parent INODE



}

void mkdir_fs(char* pathname)
{
	MINODE* pmip = NULL;
	char basename[64];
	char** names = tokenize(pathname);
	int i = 0, myino, pino;

	read_path(basename, names);

	if(basename == NULL || basename[0] == '\n')
		return;

	if(names[0] != NULL)
	{
		printf("names[0]: %s\n", names[0]);
		pino = getino(&dev, *names);
	}
	else
	{
		pino = running->cwd->ino;
	}
	printf("dev: %d, pino: %d\n", dev, pino);
	pmip = iget(dev, pino);
	
	if(!S_ISDIR(pmip->inode.i_mode))
	{
		printf("Invalid path with i_mode #%d\n", pmip->inode.i_mode);
		ip = &pmip->inode;
		print_inode();
		print_inode_contents();
		return;
	}
	
	if(search_inode(&pmip->inode, basename) != 0)
	{
		printf("Directory already exists\n");
		return;
	}

	kmkdir(pmip, basename);

	pmip->inode.i_links_count++;
	pmip->dirty = 1;
	iput(pmip);

	
}



void creat_fs(char* pathname)
{
	char filename[64];
	char** names = tokenize(pathname);
	int i = 0, ino;

	while(names[i + 1] != NULL)
	{
		i++;
	}

	if(names[i] == NULL)
	{
		printf("Invalid creat argument\n");
		return;
	}

	strcpy(filename, names[i]);	
	printf("filename: %s\n", filename);
	names[i] = NULL;

	ino = getino(&dev, *names);

	
	//	similar to mdkir except:
	//	1. inode.i_mode field is set to reg file type, permission bits set to 0644 for rw-r--r--, and


	//	2. no data block is allocated for it so the file size is 0


	//	3. Do not increment parent inodes link count
	
}

