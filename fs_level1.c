#include "alloc_dealloc.c"
#include "mkdir_creat.c"

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
				printf("Name found\n");
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
							print_dir();
							printf("cp: %d, rmdir_place: %d\n", cp, rmdir_place);
							fgets(ibuf, sizeof(ibuf), stdin);
						}
						//add deleted rec_len to last entry
						dp->rec_len += deleted_rec_len;
						
						dp = (DIR *)rmdir_place;

						//move all following directories back
						memcpy(rmdir_place, rmdir_place + dp->rec_len, remaining_blk_size);
						break;
					}
				}
			}
		}
		put_block(dev, pmip->inode.i_block[i], buf);

   }
    ip = &pmip->inode;
	print_inode_contents();
}


void ls(char* pathname)
{

	int ino;
	char* cp;
	int i_size = 0;

 	dev = running->cwd->dev;

	MINODE* mip = running->cwd;
	
	
	if(pathname[0] != '/' && pathname[0] != '\n' )
	{
		ino = getino(&dev, pathname);
		printf("ino #%d\n", ino);
		if(ino <= 0)
		{
			printf("Directory not found\n"); 
			return;
		}
		
		
		mip = iget(dev, ino);
		
	}

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
	}
	iput(mip);
}


void cd(char* pathname)
{
	MINODE* mip = NULL;
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



int rmdir_fs(char* pathname)
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

	char basename[64];
	char** names = tokenize(pathname);

	MINODE* pmip = NULL;
	MINODE* mip = iget(dev, ino);

	//verify inode is a dir

	if(!S_ISDIR(mip->inode.i_mode))
	{
		
		printf("Invalid path\n");
		ip = &pmip->inode;
		print_inode();
		print_inode_contents();
	}

	read_path(basename, names);

	//minode is not busy
	if(mip->refCount != 1)
	{
		printf("Minode is busy\n");
		return 0;
	}

	//dir only contains . and .. entries
	num_entries = get_num_entries(&mip->inode);
	if(num_entries != 2)
	{
		printf("DIR is not empty. DIR has %d entries\n", num_entries);
		return 0;
	}

	//get parents ino and inode
	pino = findino(mip); 

	 //get parent from .. entry in INODE.i_block[0]
	pmip = iget(mip->dev, pino);

	 //find name from parent DIR
	findmyname(pmip, ino, basename);

	//remove name from parent directory	
	rm_child(pmip, basename);

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


void fs_link(char* oldfile, char* newfile)
{

	MINODE *omip = NULL, *pmip = NULL;
	int oino = 0, nino = 0, pino = 0;


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
	pino = getino(&dev, newfile);
	pmip = iget(dev, pino);
	enter_child(pmip, omip->ino, basename(newfile), 7);

	omip->inode.i_links_count++;
	omip->dirty = 1;
	iput(omip);
	iput(pmip);

}

void fs_unlink(char* filename)
{
	
	int ino = 0, pino = 0;
	MINODE* mip = NULL, *pmip = NULL;

	//1. get filename's minode:
	ino = getino(&dev, filename);
	mip = iget(dev, ino);

	//2. remove basename from parent DIR
	pino = findino(mip);
	pmip = iget(dev, pino);

	rm_child(pmip, basename(filename));
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

void fs_symlink(char* oldfile, char* newfile)
{
	/*int oino = 0, nino = 0;


	MINODE* omip = NULL, *nmip = NULL;
	//1. check: old_file must exist and new_file not yet exist
	oino = getino(&dev, oldfile);
	if(oino == 0)
		return;

	omip = iget(dev, oino);

	nino = getino(&dev, newfile);
	if(nino != 0)
		return;

	nmip = iget(dev, newfile);

	//2. Change newfile to slink type  or filetype 7
	nmip->inode.i_mode = 0x81A4;
	//3. assume length of oldfile name <= 60 chars
	//	store oldfile name in newfiles inode.i_block[] area
	//	mark new files minode dirty

		iput(nmip);
	//4. mark newfile's parent minode dirty
	//	put(newfile's parent minode)

	*/
}


void fs_readlink()
{
	/*
	1. get file's inode into memory, verify it's a slink file or filetype 7
	2. copy target filename in inode.i_block into a buffer
	3. return strlen((char *)mip->inode.i_block)
	*/
}
 

