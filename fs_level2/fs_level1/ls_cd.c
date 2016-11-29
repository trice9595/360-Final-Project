

void ls(char* pathname)
{

	int ino;
	char* cp;
	char* name;
	int i_size = 0, pino;
	MINODE* pmip = NULL;

 	dev = running->cwd->dev;

	MINODE* mip = NULL;
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
		return;
	}
	
	
	mip = iget(dev, ino);


	ip = &mip->inode;

	if(!S_ISDIR(ip->i_mode))
	{

		pmip = iget(dev, file_parent_ino);
		findmyname(pmip, ino, name);
		print_dir();
		iput(pmip);
		return;
	}

	int i = 0;
	for(i = 0; i < 12; i++)
	{
		if(ip->i_block[i] != 0)
		{
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
		pathname = &pathname[1];
	}
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
				iput(mip);
				printf("File is not directory\n");
			}
		}else
		{
			printf("Directory not found\n");
		}
	}
}


