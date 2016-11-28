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

typedef struct open_file
{
	int fd;
	char *name;
}openFile;

int main(int argc, char *argv[], char *env[])
{
	int err = 0, i = 0;
	char input[256], output[256];
	char* splitInput[256];
	openFile of;

	of.name = NULL;

	err = mount_root();
	
	if (!err)
	{
		printf ("Error mounting root");

		return 0;
	}

	while (1)
	{
		printf ("Please type a command: ");
		fgets (input, 256, stdin);
		printf ("\n");

		while(i < 256)
		{
			splitInput[i] = NULL;

			i++;
		}

		i = 1;

		splitInput[0] = strtok(input, " ");

		while (splitInput[i] = strtok(NULL, " "))
		{
			i++;
		}

		if (!strcmp(splitInput[0], "ls") || !strcmp(splitInput[0], "ls\n"))
		{
			if (splitInput[1] != NULL)
			{
				ls(splitInput[1]);
			}
			else
			{
				ls(".");
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
			fs_readlink(splitInput[1], output);
		}
		else if (!strcmp(splitInput[0], "open") && splitInput[1]
&& splitInput[2])
		{
			if (of.name == NULL)
			{	
				printf("");
				of.fd = fs_open(splitInput[1], atoi(splitInput[2]));
				of.name = splitInput[1];
				printf("file descriptor: %d\n", of.fd);
			}
			else
			{
				printf("Please close previous file before opening another.\n");
			}
		}
		else if (!strcmp(splitInput[0], "close") && splitInput[1])
		{
			fs_close(of.fd);

			of.name = NULL;
		}
				else if (!strcmp(splitInput[0], "write") && splitInput[1]
&& splitInput[2])
		{
			printf("text: %s\n", splitInput[2]);
			write_file(splitInput[1], splitInput[2], strlen(splitInput[2]));

		}
		else if (!strcmp(splitInput[0], "lseek") && splitInput[1] && splitInput[2])
		{
			if (of.name != NULL)
			{
				fs_lseek(of.fd, atoi(splitInput[2]));
			}
			else
			{
				printf("File is not open.");
			}
		}
		else if (!strcmp(splitInput[0], "exit\n"))
		{
			break;
		}
		else
		{
			printf ("invalid command\n");
		}
	}
}

void mount_root()
{		
	dev = open("diskimage", O_RDWR);
	root = iget(dev, 2);

	//set processes current working directory to root minode
	proc[0].cwd = iget(root->dev, 2);
	proc[1].cwd = iget(root->dev, 2);	

	//set running process to first process
	running = &proc[0];

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

