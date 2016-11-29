

void ls(char* pathname)
{

	int ino;
	char* cp;
<<<<<<< HEAD
	char* name;
	int i_size = 0, pino;
	MINODE* pmip = NULL;
=======
	int i_size = 0;

>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea

 	dev = running->cwd->dev;

	MINODE* mip = NULL;
<<<<<<< HEAD
	if(pathname[0] == '\n' || strcmp(pathname, "\n") == 0)
	{
		pathname[0] = '.';
		pathname[1] = 0;
	}

	if(pathname[0] == '/' && pathname[1] == '\n')
	{
		ino = root->ino;
	}
	else
	{
		ino = getino(&dev, pathname);
	}

	if(ino <= 0)
	{
		printf("Directory not found\n");
=======
	if(pathname[0] == '\n' )
	{
		pathname[0] = '.';
	}

	ino = getino(&dev, basename(pathname));

	if(ino <= 0)
	{
		printf("Directory not found\n"); 
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
		return;
	}
	
	
	mip = iget(dev, ino);


	ip = &mip->inode;

<<<<<<< HEAD
	if(!S_ISDIR(ip->i_mode))
	{

		pmip = iget(dev, file_parent_ino);
		findmyname(pmip, ino, name);
		print_dir();
		iput(pmip);
		return;
	}

=======
	printf("printing directory...\n");
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
	int i = 0;
	for(i = 0; i < 12; i++)
	{
		if(ip->i_block[i] != 0)
		{
<<<<<<< HEAD
			get_block(dev, ip->i_block[i], ls_buf);
			
			cp = ls_buf;
		    while (cp < (ls_buf + BLKSIZE))
			{
		   		dp = (DIR *)cp;
				if(dp->rec_len <= 0 || dp->rec_len > 1024)
				{
					break;
				}

				print_dir();

				cp += dp->rec_len;
=======
			get_block(dev, ip->i_block[i], buf);
			
			cp = buf;
		    while (cp < buf + BLKSIZE)
			{
		   		dp = (DIR *)cp;
				if(dp->rec_len <= 0)
				{
				printf("ERROR: dp->rec_len of %d is invalid\n", dp->rec_len);
					break;
				}
				print_dir();
				cp += dp->rec_len;


>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
		    }
			
		}
		else
		{
			//printf("i_block[%d] is empty\n", i);
		}
	}
	iput(mip);
}


void cd(char* pathname)
{
	char* base = NULL;
	MINODE* mip = NULL, *omip = running->cwd;
	int ino;
	if(pathname[0] == '/')
	{
		running->cwd = root;
<<<<<<< HEAD
		pathname = &pathname[1];
	}
=======
	printf("pathname: %s\n", pathname);
		pathname = &pathname[1];
	}
	printf("pathname: %s\n", pathname);
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
	if(pathname != NULL && pathname[0] != '\n')
	{
		ino = getino(&dev, pathname);

		if(ino != 0)
		{
			
			mip = iget(dev, ino);
			
			if(S_ISDIR(mip->inode.i_mode))
			{
				running->cwd = mip;
				iput(omip);
			}
			else
			{
<<<<<<< HEAD
				iput(mip);
=======
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
				printf("File is not directory\n");
			}
		}else
		{
			printf("Directory not found\n");
		}
	}
}


