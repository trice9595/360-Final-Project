
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
	mip->inode.i_size = BLKSIZE;
	mip->inode.i_atime = (u32)time(NULL);
	mip->dirty = 1;
	iput(mip);


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


void fs_mkdir(char* pathname)
{
	char base[64];
	strcpy(base, basename(pathname));
	MINODE* pmip = NULL;

	pmip = get_parent_minode(pathname);



	if(base[strlen(base) - 1] == '\n')
	{
		base[strlen(base) - 1] = '\0';
	}

	if(pmip != NULL)
	{
		kmkdir(pmip, base);

		pmip->inode.i_links_count++;
		pmip->dirty = 1;
		iput(pmip);
	}
}


void kcreat(MINODE* pmip, char* basename)
{
	char* cp;
	int i, ino = ialloc(dev);
	MINODE* mip = iget(dev, ino);
	
	//do I need to mark it dirty and put it

	mip->dirty = 1;

	//make data block 0 of inode to contain . and .. entries

	//test if done correctly

	mip->inode.i_mode = 0x81A4;
	mip->inode.i_size = 0;
	mip->inode.i_atime = (u32)time(NULL);
	
	iput(mip);

	enter_child(pmip, ino, basename, 1);
	

    //which enters (ino, basename) as a DIR entry to the parent INODE

}

void fs_creat(char* pathname)
{
	char base[64];
	MINODE* pmip = NULL;

	strcpy(base, basename(pathname));
	pmip = get_parent_minode(pathname);

	if(base[strlen(base) - 1] == '\n')
	{
		base[strlen(base) - 1] = '\0';
	}

	if(pmip != NULL)
	{
		kcreat(pmip, base);

		pmip->inode.i_links_count++;
		pmip->dirty = 1;
		iput(pmip);
	}
}

