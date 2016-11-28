//custom file system
//John Howley and Tim Rice
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void split_command(char *pathname, char **names)
{
	int i = 0;
	//char** names = malloc(64*sizeof(char *));	
	const char* delim = " ";
	
	names[i] = strtok(pathname, delim);
	while(names[i] != NULL)
	{
		i++;
		names[i] = strtok(NULL, delim);
	}
	names[i - 1] = strtok(names[i - 1], "\n");
}

int main(int argc, char *argv[], char *env[])
{
	int err = 0, i = 0;
	char input[256];
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
		printf("this is the input: %s\n", input);
		printf ("\n");

		while(i < 256)
		{
			splitInput[i] = NULL;

			i++;
		}

		i = 0;
		/*
		splitInput[0] = strtok(input, " ");
		printf("splitInput[0]: %s\n", splitInput[0]);

		splitInput[1] = strtok(NULL, " ");
		printf("splitInput[1]: %s\n", splitInput[1]);

		while (splitInput[i] = strtok(NULL, " "))
		{
			i++;
			printf("splitInput[i]: %s\n", splitInput[i]);
		}*/
		split_command(input, splitInput);

		while (splitInput[i])
		{
			printf("splitInput[%d]: %s\n", i, splitInput[i]);
			i++;
		}

		if (!strcmp(splitInput[0], "ls"))
		{
			if (splitInput[1])
			{
				ls(splitInput[1]);
			}
			else
			{
				ls("\n");
			}
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "cd"))
		{
			cd(splitInput[1]);
		}
		else if (!strcmp(splitInput[0], "pwd"))
		{
			//pwd();
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "mkdir"))
		{
			mkdir_fs(splitInput[1]);
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "rmdir"))
		{
			rmdir_fs(splitInput[1]);
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "creat"))
		{
			creat_fs(splitInput[1]);
		}
		else if (splitInput[1] && splitInput[2] && !strcmp(splitInput[0], "link"))
		{
			fs_link(splitInput[1], splitInput[2]);
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "unlink"))
		{
			fs_unlink(splitInput[1]);
		}
		else if (splitInput[1] && splitInput[2] && !strcmp(splitInput[0], "symlink"))
		{
			fs_symlink(splitInput[1], splitInput[2]);
		}
		else if (!strcmp(splitInput[0], "readlink"))
		{
			//fs_readlink();
		}
		else if (!strcmp(splitInput[0], "open") && splitInput[1]
&& splitInput[2])
		{
			fs_open(splitInput[1], splitInput[2]);
		}
		else if (!strcmp(splitInput[0], "close") && splitInput[1])
		{
			fs_close(splitInput[1]);
		}
		else if (!strcmp(splitInput[0], "lseek") && splitInput[1] && splitInput[2])
		{
			fs_lseek(splitInput[1], splitInput[2]);
		}
		else if (!strcmp(splitInput[0], "exit"))
		{
			break;
		}
		else
		{
			printf ("invalid command\n");
		}
	}
}

int mount_root()
{
	dev = open("diskimage", O_RDONLY);
	root = iget(dev, 2);

	proc[0].cwd = iget(root->dev, 2);
	proc[1].cwd = iget(root->dev, 2);	

	running = &proc[0];

	if (!dev && !root && !running)
	{
		return 0;
	}

	return 1;
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

