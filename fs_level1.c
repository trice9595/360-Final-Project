#include "util.c"


void ls(char* pathname)
{

	int ino;
	int i_size = 0;
	
 	dev = running->cwd->dev;

	MINODE* mip = running->cwd;
	
	if(pathname[0] != '/' && pathname[0] != '\n' )
	{
	
		ino = getino(dev, pathname);
		printf("got ino: %d\n", ino);
		
		mip = iget(dev, ino);
		printf("got minode!\n");
	}

	ip = &mip->inode;
	i_size = ip->i_size;

	printf("printing directory...\n");
	int i = 0;
	for(i = 0; i < 12; i++)
	{
		if(ip->i_block[i] != 0)
		{
			get_block(3, ip->i_block[i], buf);
			dp = (DIR *)buf;
			i_size -= dp->rec_len;

			print_dir();
			while(dp != NULL && i_size > 0)
			{
				dp = (DIR *)((char *)dp + dp->rec_len);
				print_dir();
				i_size -= dp->rec_len;
			}
			
			if(i_size <= 0)
				break;
		}
	}
}


void cd(char* pathname)
{
	int ino;
	if(pathname == NULL || strcmp(pathname, "/") == 0)
	{
		running->cwd = root;
	}
	else
	{
		ino = getino(&dev, pathname);
		running->cwd = iget(dev, ino);
	}
}

void mkdir_fs(char* pathname)
{
	MINODE* pmip = NULL;
	char basename[64];
	char** names = tokenize(pathname);
	int i = 0, ino;

	//if path is absolute
	if(pathname[0] == "/")
		dev = root->dev;
	else
		dev = running->cwd->dev;


	while(names[i + 1] != NULL)
	{
		i++;
	}

	
	if(names[i] == NULL)
	{
		printf("Invalid mkdir argument\n");
		return;
	}

	strcpy(basename, names[i]);	
	printf("basename: %s\n", basename);
	names[i] = NULL;

	ino = getino(&dev, basename);
	pmip = iget(dev, ino);
	
	if(pmip->inode.i_mode != 2)
	{
		printf("Invalid path\n");
		return;
	}
	
	if(search_inode(pmip, basename) != 0)
	{
		printf("Directory already exists\n");
		return;
	}

	kmkdir(pmip, basename);

	pmip->inode.i_links_count++;
	pmip->dirty = 1;
	iput(pmip);

}

void kmkdir(MINODE* pmip, char* basename)
{
	int ino = ialloc(dev);
	MINODE* mip = iget(dev, ino);
	
	mip->inode.i_block[0] = blk;
	mip->dirty = 1;
	iput(mip);

	//make data block 0 of inode to contain . and .. entries
	//write to disk block blk
	//enter_child(pmip, ino, basename);
    //which enters (ino, basename) as a DIR entry to the parent INODE


}


int rmdir_fs(pathname)
{
	//get in-memory inode
	int num_entries;
	int pino;
	int ino = getino(&dev, pathname);
	MINODE* pmip = NULL;
	MINODE* mip = iget(dev, ino);

	//verify inode is a dir
	if(mip->inode.i_mode != 2)
	{
		printf("Invalid path\n");
		return 0;
	}

	//minode is not busy
	if(mip->refCount != 1)
	{
		printf("Minode is busy\n");
		return 0;
	}

	//dir only contains . and .. entries
	num_entries = get_num_entries(mip->inode);
	if(num_entries != 2)
	{
		printf("DIR is not empty. DIR has %d entries\n", num_entries);
		return 0;
	}

	//get parents ino and inode
	pino = find_ino(); //get parent from .. entry in INODE.i_block[0]
	pmip = iget(mip->dev, pino);
	
	//remove name from parent directory
	findname(pmip, ino, name); //find name from parent DIR
	rm_child(pmip, name);

	//deallocate its data blocks and inode
	truncat(mip);
	
	//deallocate INODE
	idalloc(mip->dev, mip->ino);
	iput(mip);

	pmip->inode.i_links_count--;
	pmip->dirty = 1;
	iput(pmip);

	return 1;
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

	ino = getino(*names);

	/*
		similar to mdkir except:
		1. inode.i_mode field is set to reg file type, permission bits set to 0644 for rw-r--r--, and
		2. no data block is allocated for it so the file size is 0
		3. Do not increment parent inodes link count
	*/
}
