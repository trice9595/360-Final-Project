#include "alloc_dealloc.c"
#include "mkdir_creat.c"

void fs_truncate(MINODE* mip)
{
	int i = 0;

	ip = &mip->inode;
	
	for (i = 0; i < 12; i++)
	{
		ip->i_block[i];
	}

}

int rm_child(MINODE* pmip, char* name)
{
	

}

void ls(char* pathname)
{

	int ino;
	int i_size = 0;
	
 	dev = running->cwd->dev;

	MINODE* mip = running->cwd;
	
	if(pathname[0] != '/' && pathname[0] != '\n' )
	{
	
		ino = getino(&dev, pathname);
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



int rmdir_fs(char* pathname)
{
	//get in-memory inode
	int num_entries;
	int pino;
	int ino = getino(&dev, pathname);

	char basename[64];
	char** names = tokenize(pathname);

	MINODE* pmip = NULL;
	MINODE* mip = iget(dev, ino);

	//verify inode is a dir
	if(mip->inode.i_mode != 2)
	{
		printf("Invalid path\n");
		return 0;
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
	findino(mip, &ino, &pino); //get parent from .. entry in INODE.i_block[0]
	pmip = iget(mip->dev, pino);

	 //find name from parent DIR
	findmyname(pmip, ino, basename);

	//remove name from parent directory
		

	rm_child(pmip, basename);

	//deallocate its data blocks and inode
	fs_truncate(pmip);
	
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

	/*1. verrify oldfile and is not DIR
	int oino = getino(&odev, oldfile);
	MINODE* omip = iget(odev, oldfile)

	2. new file must not exist yet
	int nino = getino(&ndev, newfile); must retu
	if(nino != 0)
		return;

	3. creat entry in new_parent DIR with sae ino
	pmip -> minode of dirname(newfile)
	enter_name(pmip, omip->ino, basename(newfile))

	4. omip->inode.i_links_count++;
	omip->dirty = 1;
	iput(omip);
	iput(omip);
	*/
}

void fs_unlink(char* filename)
{
	/*
	1. get filename's minode:
	int ino = getino(&dev, filename)
	MINODE* mip = iget(dev, ino)

	2. remove basename from parent DIR
	rm_child(pmip, mip->ino, basename)
	pmip->dirty = 1;
	iput(pmip);

	3.decrement inode's link count
	mip->inode.i_links_count--;
	if(mip->inode.links_count > 0)
		mip->dirty = 1; iput(mip);

	if(!SLINK file)  or filetype 7 //assume:SLINK file has not data block
		truncate(mip);
	deallocate(inode);
	iput(mip);

	*/
}

void fs_symlink()
{
	/*
	1. check: old_file must exist and new_file not yet exist
	2. create newfile; change newfile to slink type  or filetype 7
	3. assume length of oldfile name <= 60 chars
		store oldfile name in newfiles inode.i_block[] area
		mark new files minode dirty
		iput(newfile's minode)
	4. mark newfile's parent minode dirty
		put(newfile's parent minode)
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
 

