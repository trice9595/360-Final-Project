

void enter_child(MINODE* pmip, int ino, char* basename)
{
	int i, old_rec_len = 0;
	char *cp;
	char exBuf[10];

	printf("entering child...\n");
	//ip = ;
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (pmip->inode.i_block[i] == 0)
		{
          return;
		}

       get_block(dev, pmip->inode.i_block[i], buf);
		
       cp = buf;
       while (cp < buf + BLKSIZE){
          // print dp->inode, dp->rec_len, dp->name_len, dp->name);	  
          dp = (DIR *)cp;
	      print_dir();
		fgets(exBuf, sizeof(exBuf), stdin);
          cp += dp->rec_len;
       }

		old_rec_len = dp->rec_len;
		old_rec_len -= dp->name_len + 12;

		printf("strlen(basename): %d\n", strlen(basename));
		if(old_rec_len >= strlen(basename) + 8)
		{
			
			dp->rec_len = dp->name_len + 12;
			dp = (DIR *)((char *)dp + dp->rec_len);

			strcpy(dp->name, basename);
			dp->name_len = strlen(basename);
			dp->rec_len = old_rec_len;
			dp->file_type = 2;
			dp->inode = ino;
			print_dir();
			
			put_block(dev, pmip->inode.i_block[i], buf);
			break;
		}
		else
		{
			printf("\n\n****i_block is full***\n\n");
		}
	}
	
}

int char_arr_cmp(char buf[1024], char buf2[1024])
{
	int count = 0;
	for(int i = 0; i < 1024; i++)
	{
		if(buf[i] != buf2[i])
		{
			count++;
		}
	}
	return count;
}


void kmkdir(MINODE* pmip, char* basename)
{
	char* cp;
	int i, ino = ialloc(dev), blk = balloc(dev);
	MINODE* mip = iget(dev, ino);
	
	mip->inode.i_block[0] = blk;
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
	dp->rec_len = 12;
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
	printf("here\n");
	if(names[0] != NULL)
	{
		printf("names[0]: %s\n", names[0]);
		pino = getino(&dev, *names);
	}
	else
	{
		pino = running->cwd->ino;
	}
	pmip = iget(dev, pino);
	
	if(!S_ISDIR(pmip->inode.i_mode))
	{
		printf("Invalid path with i_mode #%d\n", pmip->inode.i_mode);
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

