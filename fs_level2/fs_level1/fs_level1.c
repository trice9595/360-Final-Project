#include <libgen.h>
#include "alloc_dealloc.c"
#include "fs_level1_help.c"
#include "ls_cd.c"
#include "rm_rmdir.c"
#include "link_unlink.c"
#include "mkdir_creat.c"
#include <time.h>

/*
Include all level 1 functions
*/


int chmod_user(char mode, MINODE* mip)
{

	mip->inode.i_mode = mip->inode.i_mode & 0x003F;

	if(mode & 1)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0040; 
	}
	if(mode & 2)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0080; 
	}
	if(mode & 4)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0100; 
	}

}

void chmod_group(char mode, MINODE* mip)
{
	mip->inode.i_mode = mip->inode.i_mode & 0xFFC7;

	if(mode & 1)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0008; 
	}
	if(mode & 2)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0010; 
	}
	if(mode & 4)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0020; 
	}
}

void chmod_other(char mode, MINODE* mip)
{
	mip->inode.i_mode = mip->inode.i_mode & 0xFFF8;

	if(mode & 1)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0001; 
	}
	if(mode & 2)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0002; 
	}
	if(mode & 4)
	{
		mip->inode.i_mode = mip->inode.i_mode | 0x0004; 
	}
}

void fs_chmod(char* mod, char* filename)
{
	MINODE* mip = NULL;
	int ino = 0;
	int i = 0;

	ino = getino(&dev, filename);
	mip = iget(dev, ino);

	if(i < strlen(mod))
	{
		chmod_other(mod[strlen(mod) - 1 - i], mip);
	}
	i++;

	if(i < strlen(mod))
	{
		chmod_group(mod[strlen(mod) - 1 - i], mip);
	}
	i++;

	if(i < strlen(mod))
	{
		chmod_user(mod[strlen(mod) - 1 - i], mip);
	}
	i++;
	mip->dirty = 1;
	iput(mip);
}


void fs_pwd()
{
	int ino = 0, pino = 0, i = -1;
	char parents[16][128];

	MINODE* mip = running->cwd;
	MINODE* pmip = NULL;
	ino = mip->ino;

	if(ino == root->ino)
	{
		printf("/\n");
		return;
	}
	
	pino = findino(mip);

	do
	{
		i++;

		pmip = iget(dev, pino);

		findmyname(pmip, ino, parents[i]);
		strcpy(parents[i], dp->name);

		pino = findino(pmip);
		ino = pmip->ino;

	}while(ino != root->ino);

	while(i >= 0)
	{
		printf("/");
		printf("%s", parents[i]);
		i--;
	}
	printf("/\n");

	iput(pmip);
}


void fs_touch(char* filename)
{
	MINODE* mip = NULL;
	int ino = getino(&dev, filename);

	//if file exists
	if(ino == 0)
	{
		fs_creat(filename);
	}
	else
	{
		mip = iget(dev, ino);
		mip->inode.i_atime = (u32)time(NULL);
		mip->dirty = 1;
		iput(mip);
	}

}


