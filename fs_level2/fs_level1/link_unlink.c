
void fs_link(char* oldfile, char* newfile)
{


	MINODE *omip = NULL, *pmip = NULL, *nmip = NULL;
	int oino = 0, nino = 0, pino = 0;
	char newdir[128];
	char base[128];
	strcpy(base, basename(newfile));
	strcpy(newdir, dirname(newfile));


	//1. verify oldfile and is not DIR
	oino = getino(&dev, oldfile);

	if(oino == 0)
	{
		printf("File does not exist\n");
		return;
	}
	

	omip = iget(dev, oino);

	if(S_ISDIR(omip->inode.i_mode))
	{
		printf("File is directory\n");	
		return;
	}

	//2. new file must not exist yet
	nino = getino(&dev, newfile);
	if(nino != 0)
		return;
		

	//3. creat entry in new_parent DIR with same ino
	//pmip -> minode of dirname(newfile)

	if(strcmp(newdir, ".") != 0)
	{
		pino = getino(&dev, newdir);
	}
	else
	{
		pino = running->cwd->ino;
	}


	if(pino <= 0)
	{
		printf("pino not found\n");
		return;
	}
	
	nmip = iget(dev, nino);
	nmip->dirty = 1;

	nmip->inode.i_mode = 0x81A4;
	nmip->inode.i_size = omip->inode.i_size;
	nmip->inode.i_atime = (u32)time(NULL);
	
	iput(nmip);

	pmip = iget(dev, pino);

	if(base[strlen(base) - 1] == '\n')
	{
		base[strlen(base) - 1] = '\0';
	}

	enter_child(pmip, omip->ino, base, 7);

	omip->inode.i_links_count++;
	omip->dirty = 1;
	iput(omip);
	iput(pmip);

}

void fs_unlink(char* filename)
{
	
	int ino = 0, pino = 0;
	MINODE* mip = NULL, *pmip = NULL;

	char base[128];
	strcpy(base, basename(filename));
	char* dir = dirname(filename);
	

	//1. get filename's minode:
	ino = getino(&dev, filename);
	mip = iget(dev, ino);


	//2. remove basename from parent DIR
	if(strcmp(dir, ".") != 0)
	{
		pino = getino(&dev, dir);
	}
	else
	{
		pino = running->cwd->ino;
	}

	pmip = iget(dev, pino);


	if(base[strlen(base) - 1] == '\n')
	{
		base[strlen(base) - 1] = '\0';
	}

	rm_child(pmip, base);
	pmip->dirty = 1;
	iput(pmip);

	//3.decrement inode's link count
	mip->inode.i_links_count--;
	if(mip->inode.i_links_count > 0)
	{
		mip->dirty = 1;
		iput(mip);
	}

     //assume:SLINK file has not data block
	if(S_ISLNK(!mip->inode.i_mode))
		fs_truncate(mip);

	idealloc(dev, mip->ino);
	iput(mip);

	
}


//create generic file creation process that symlink, link, creat, and mkdir all call
void fs_symlink(char* oldfile, char* newfile)
{
	char* cp;
	int i = 0;
	int oino = 0, nino = 0, pino, blk;

	char base[128];
	strcpy(base, basename(newfile));

	char* dir = dirname(newfile);

	MINODE* omip = NULL, *nmip = NULL, *pmip = NULL;
	//1. check: old_file must exist and new_file not yet exist
	oino = getino(&dev, oldfile);
	if(oino == 0)
		return;

	
	omip = iget(dev, oino);

	nino = getino(&dev, newfile);
	if(nino != 0)
		return;

	nino = ialloc(dev);
	blk = balloc(dev);
	nmip = iget(dev, nino);
	
	//do I need to mark it dirty and put it
	nmip->inode.i_block[0] = blk;
	for(i = 1; i < 12; i++)
	{
		nmip->inode.i_block[i] = 0;
	}


	nmip->inode.i_atime = (u32)time(NULL);
	nmip->inode.i_size = BLKSIZE;
	nmip->inode.i_mode = 0xA1A4;
	nmip->dirty = 1;
	iput(nmip);



	pmip = get_parent_minode(newfile);

	if(base[strlen(base) - 1] == '\n')
	{
		base[strlen(base) - 1] = '\0';
	}

	enter_child(pmip, nino, base, 7);


	//3. assume length of oldfile name <= 60 chars
	//	store oldfile name in newfiles inode.i_block[] area
	//	mark new files minode dirty
	get_block(dev, nmip->inode.i_block[0], buf);

	memcpy(buf, oldfile, strlen(oldfile));

	put_block(dev, nmip->inode.i_block[0], buf);

	nmip->dirty = 1;


	iput(omip);

	pmip->dirty = 1;
	iput(pmip);
}


int fs_readlink(char* pathname, char buffer[])
{
	
	//1. get file's inode into memory, verify it's a slink file
	int ino = getino(&dev, pathname);
	MINODE* mip = iget(dev, ino);
	//2. copy target filename in inode.i_block into a buffer
	get_block(dev, mip->inode.i_block[0], buffer);
	printf("target filename: %s\n", buffer);

	iput(mip);

	//3. return strlen((char *)mip->inode.i_block)
	return strlen((char *)buffer);
	
}
 

