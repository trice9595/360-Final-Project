

void ls(char* pathname)
{

	int ino;
	char* cp;
	int i_size = 0;


 	dev = running->cwd->dev;

	MINODE* mip = NULL;
	if(pathname[0] == '\n' )
	{
		pathname[0] = '.';
	}

	ino = getino(&dev, basename(pathname));

	if(ino <= 0)
	{
		printf("Directory not found\n"); 
		return;
	}
	
	
	mip = iget(dev, ino);


	ip = &mip->inode;

	printf("printing directory...\n");
	int i = 0;
	for(i = 0; i < 12; i++)
	{
		if(ip->i_block[i] != 0)
		{
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
	MINODE* mip = NULL, *omip = running->cwd;
	int ino;
	if(pathname == NULL || strcmp(pathname, "/") == 0 || 
		pathname[0] == '\n')
	{
		running->cwd = root;
	}
	else
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
				printf("File is not directory\n");
			}
		}else
		{
			printf("Directory not found\n");
		}
	}
}


