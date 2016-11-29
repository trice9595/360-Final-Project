#include <libgen.h>
#include "alloc_dealloc.c"
#include "fs_level1_help.c"
#include "ls_cd.c"
#include "rm_rmdir.c"
#include "link_unlink.c"
#include "mkdir_creat.c"

/*
Include all level 1 functions
*/

void fs_chmod(u16 mod, char* filename)
{
	MINODE* mip = NULL;
	int ino = 0;

	ino = getino(&dev, filename);
	mip = iget(dev, ino);

	mip->inode.i_mode = mod;
}


void fs_pwd()
{
	

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
		mip->inode.i_atime = (unsigned)time(NULL);
	}

}


