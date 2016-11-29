#include "fs_level1/fs_level1.c"
#include "open_close.c"
#include "read_write.c"

int fs_lseek(int fd, int position)
{
	//set file's OFT to beginning of file + postition
	int mode = running->fd[fd]->mode;
	running->fd[fd]->offset = position;

	MINODE* mip = running->fd[fd]->mptr;
	if(mode == 0 && position > mip->inode.i_size)
	{
		printf("Cannot set seek past file size while in read mode\n");	
	}
	else
	{
		running->fd[fd]->offset = position;
	}
}


void fs_cat(char* filename)
{
	char mybuf[1024], dummy = 0;
	int n;

	int fd = fs_open(filename, 0);
	while(n = read_file(fd, mybuf, 1024))
	{
		mybuf[n] = 0;	
		printf("%s", mybuf); //add \n handling
	}
	fs_close(fd);
}

void fs_cp(char* src, char* dest)
{
	char mybuf[1024], dummy = 0;
	int n;

	int fd = fs_open(src, 0);
	int gd = fs_open(dest, 1);
	printf("fd: %d\ngd: %d\n", fd, gd);
	if(gd < 0)
	{
		fs_creat(dest);
		gd = fs_open(dest, 1);
		printf("gd: %d\n", gd);
	}

	while(n = read_file(fd, mybuf, 1024))
	{
		write_file(gd, mybuf, n); //add \n handling
	}
	fs_close(fd);
	fs_close(gd);
}

int fs_mv(char* src, char* dest)
{
	int dest_ino, src_ino = getino(&dev, src);
	if(src_ino == 0)
		return -1;


	dest_ino = getino(&dev, dest);
	if(dest_ino != 0)
	{
	//case 1: same dev
		fs_link(src, dest);
		fs_unlink(src);
	}
	else
	{
	//case 2: different dev
		fs_cp(src, dest);
		fs_unlink(src);
	}
}
