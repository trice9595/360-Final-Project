//custom file system
//John Howley and Tim Rice
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_level1.c"
#include "fs_level2.c"


SUPER *sp = NULL;
GD    *gp = NULL;
INODE *ip = NULL;
DIR   *dp = NULL;

MINODE minode[NMINODE] = {0};
PROC   proc[NPROC] = {0}, *running = NULL;
MINODE *root = NULL;

int dev = 0, imap = 0, bmap = 0;
int ninodes = 0, nblocks = 0;
int blk = 0, offset = 0;
int inodes_begin_block = 0, inodes_per_block = 0;
char buf[BLKSIZE] = { 0 };
char buf2[BLKSIZE] = { 0 };

 int main(int argc, char *argv[], char *env[])
  {
 	int err = 0, i = 0;
 	char* input;
 	char* splitInput[256];
 
 	err = mount_root();
 	
 	if (!err)
 	{
 		printf ("Error mounting root");
 
 		return 0;
 	}
 
 	while (1)
 	{
 		printf ("Please type a command: ");
 		scanf ("%s", input);
 		printf ("\n");
 
 		while(i < 256)
 		{
 			splitInput[i] = NULL;
 
  			i++;
  		}

  		while (splitInput[i] = strtok(NULL, " "))
 		i = 0;
 	
 		while (splitInput[i] = strtok(NULL, " ")
  		{
  			i++;
  		}
 
 		if (!strcmp(splitInput[0], "ls"))
 		{
 			ls(splitInput[1]);
 		}
 		else if (splitinput[1] && !strcmp(splitInput[0], "cd"))
 		{
 			cd(splitInput[1]);
 		}
 		else if (!strcmp(splitInput[0], "pwd"))
 		{
 			pwd();
 		}
 		else if (splitInput[1] && !strcmp(splitInput[0], "mkdir"))
 		{
 			mkdir_fs(splitInput[1]);
 		}
 		else if (splitInput[1] && !strcmp(splitInput[0], "rmdir"))
 		{
 			rmdir_fs(splitInput[1]);
 		}
 		else if (splitInput1 && !strcmp(splitInput[0], "creat"))
  		{
  			creat_fs(splitInput[1]);
  		}
		else if (splitInput[1] && splitInput[2] && !strcmp(splitInput[0], "link"))
  		{
  			fs_link(splitInput[1], splitInput[2]);
  		}
 		else if (splitInput[1] && splitInput[2] && !strcmp(splitInput[0], "unlink"))
  		{
  			fs_unlink(splitInput[1], splitInput[2]);
  		}
 		else if (splitInput[1] && splitInput[2] && !strcmp(splitInput[0], "symlink"))
  		{
  			fs_symlink(splitInput[1], splitInput[2]);
  		}
  		else if (!strcmp(splitInput[0], "readlink"))
  		{
  			fs_readlink();
  		}
 		else if (!strcmp(splitInput[0], "open") && splitInput[1])
 		{
 			fs_open(splitInput[1]);
 		}
 		else if (!strcmp(splitInput[0], "close") && splitInput[1])
 		{
 			fs_close(splitInput[1]);
 		}
 		else if (!strcmp(splitInput[0], "lseek") && splitInput[1] && splitInput[2])
 		{
 			fs_lseek(splitInput[1], splitInput[2]);
 		}
  		else
  		{
  			printf ("invalid command\n");
 		}
 	}
 }

void init()
{
	
	int i = 0;


	proc[0].uid = 0;
	proc[1].uid = 1;
	proc[0].cwd = 0;
	proc[1].cwd = 0;


	for(i = 0; i < NMINODE; i++)
	{
		minode[i].refCount = 0;
	}

}

void mount_root()
{		
	dev = open("mydisk", O_RDWR);
	root = iget(dev, 2);

	//set processes current working directory to root minode
	proc[0].cwd = iget(root->dev, 2);
	proc[1].cwd = iget(root->dev, 2);	

	//set running process to first process
	running = &proc[0];

}
