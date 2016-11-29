
void fs_truncate(MINODE* mip)
{
	int i = 0;

	ip = &mip->inode;
	
	for (i = 0; i < 12; i++)
	{
		bdealloc(dev, ip->i_block[i]);
		ip->i_block[i] = 0;
	}

}

int add_dir_entry(int ino, char* basename, int rec_len, int file_type)
{
	strcpy(dp->name, basename);
	dp->name_len = strlen(basename);
	dp->rec_len = rec_len;
	dp->file_type = file_type;
	dp->inode = ino;

}

int get_last_entry(char* cp)
{
   while (cp < buf + BLKSIZE)
	{
   		dp = (DIR *)cp;
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
			pmip->inode.i_block[i + 1] = balloc(dev);

		}
	}
	
}


int get_permission(MINODE* mip)
{
	return mip->inode.i_mode;
}



MINODE* get_parent_minode(char* pathname)
{

	MINODE* pmip = NULL;
	char base[64], dir[64];

	strcpy(base, basename(pathname));
	strcpy(dir, dirname(pathname));

	int pino = 0;


	if(base[strlen(base) - 1] == '\n')
	{
		base[strlen(base) - 1] = '\0';
	}

	if(base == NULL || strcmp(base, ".") == 0)
		return NULL;

	

	pino = getino(&dev, dir);

	pmip = iget(dev, pino);
	
	if(!S_ISDIR(pmip->inode.i_mode))
	{
		printf("Invalid path with i_mode #%d\n", pmip->inode.i_mode);
		return NULL;
	}
	if(search_inode(&pmip->inode, base) != 0)
	{
		printf("File already exists\n");
		return NULL;
	}

	return pmip;
}

int rm_child(MINODE* pmip, char* name)
{
	char* cp, ibuf[10];
	char* prev_dir;
	int i = 0, prev_rec_len = 0, last_rec_len = 0;
	int remaining_blk_size = 0, deleted_rec_len = 0;
	char* rmdir_place = 0;
	DIR copy;	

	for(i = 0; i < 12; i++)
	{
		if(pmip->inode.i_block[i] == 0)
			break;

        get_block(dev, pmip->inode.i_block[i], buf);
		
        cp = buf;

		while (cp < buf + BLKSIZE)
		{
	   		dp = (DIR *)cp;
			if(dp->rec_len <= 0)
			{
				printf("ERROR: dp->rec_len of %d is invalid\n", dp->rec_len);
				break;
			}

			if(strcmp(dp->name, name) != 0)
			{
				prev_rec_len = dp->rec_len;
				cp += dp->rec_len;
			}
			else
			{
				//only entry in block
				if(dp->rec_len == BLKSIZE)
				{
					dp->inode = 0;
				}
				else
				{
					//last entry in block
					if(cp + dp->rec_len >= buf + BLKSIZE)
					{
						last_rec_len = dp->rec_len;
						cp -= prev_rec_len;
						dp = (DIR *)cp;

						dp->rec_len += last_rec_len;
					}
					//middle of block somewhere
					else
					{

						//set place of dir to copy over
						rmdir_place = cp;
						deleted_rec_len = dp->rec_len;

						//get length of block after deletion
						remaining_blk_size = buf + BLKSIZE - cp;

						//find last entry
						while (cp + dp->rec_len < buf + BLKSIZE)
						{
							cp += dp->rec_len;
							dp = (DIR *)cp;

							if(dp->rec_len <= 0)
							{
								printf("rec_len is less than or equal to zero\n");
								break;
							}
						}
						//add deleted rec_len to last entry
						dp->rec_len += deleted_rec_len;
						dp = (DIR *)rmdir_place;

						//move all following directories back
						memmove(rmdir_place, rmdir_place + dp->rec_len, remaining_blk_size);
						break;
					}
				}
			}
		}
		put_block(dev, pmip->inode.i_block[i], buf);

   }
}



