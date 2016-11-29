
//custom file system
//John Howley and Tim Rice
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_level2/fs_level2.c"


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
<<<<<<< HEAD
int file_parent_ino = 0;
int inodes_begin_block = 0, inodes_per_block = 0;
char buf[BLKSIZE] = { 0 };
char buf2[BLKSIZE] = { 0 };
char ls_buf[BLKSIZE] = { 0 };
=======
int inodes_begin_block = 0, inodes_per_block = 0;
char buf[BLKSIZE] = { 0 };
char buf2[BLKSIZE] = { 0 };

>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea

typedef struct open_file
{
	int fd;
	char *name;
}openFile;

void mount_root()
{		
<<<<<<< HEAD
	dev = open("diskimage", O_RDWR);
=======
	dev = open("mydisk", O_RDWR);
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
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

<<<<<<< HEAD

=======
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
int main(int argc, char *argv[], char *env[])
{
	int err = 0, i = 0, k = 0;
	char input[256] = {0}, output[256], frankenstring[256] = {0}, output2[4096];
	char* splitInput[32];
	openFile of;

	of.name = NULL;


	init();
<<<<<<< HEAD
	 mount_root();

=======
	/*err =*/ mount_root();
	/*if (!err)
	{
		printf ("Error mounting root");
		return 0;
	}*/
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea

	while (1)
	{
		printf ("Please type a command: ");
		fgets (input, 256, stdin);
		printf ("\n");		

		for(i = 0; i < 32; i++)
		{
			frankenstring[i] = '\0';

			splitInput[i] = NULL;
		}

<<<<<<< HEAD
		i = 0;
		
		for(i = 0; i < 4096; i++)
		{
			output2[i] = 0;
		}

		i = 1;
	
=======
		i = 1;
		

>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
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
<<<<<<< HEAD
		else if (!strcmp(splitInput[0], "pwd\n"))
		{
			fs_pwd();
		}		
		else if (!strcmp(splitInput[0], "pfd\n"))
		{
			pfd();
=======
		else if (!strcmp(splitInput[0], "pwd"))
		{
			fs_pwd();
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "mkdir"))
		{
			fs_mkdir(splitInput[1]);
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "rmdir"))
		{
			fs_rmdir(splitInput[1]);
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "creat"))
		{
			fs_creat(splitInput[1]);
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
<<<<<<< HEAD

			of.fd = fs_open(splitInput[1], atoi(splitInput[2]));	
			if(of.fd < 0)
			{
				printf("Error opening file\n");
=======
			if (of.name == NULL)
			{	
				of.fd = fs_open(splitInput[1], atoi(splitInput[2]));
				of.name = splitInput[1];
				printf("file descriptor: %d\n", of.fd);
			}
			else
			{
				printf("Please close previous file before opening another.\n");
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
			}
		}
		else if (!strcmp(splitInput[0], "close") && splitInput[1])
		{
<<<<<<< HEAD
			fs_close(atoi(splitInput[1]));
=======
			fs_close(of.fd);
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea

			of.name = NULL;
		}
		else if (!strcmp(splitInput[0], "write") && splitInput[1]
&& splitInput[2])
		{
			if (*splitInput[2] == '"')
			{
				for (int j = 2; splitInput[j] != NULL; j++)
				{
					for (int m = 0; splitInput[j][m] != '\0'; m++)
					{
						if (splitInput[j][m] == '"'){}
						else
						{
							frankenstring[k] = splitInput[j][m];
							k++;
						}
					}

					frankenstring[k] = ' ';

					k++;
				}

				printf("text: %s\n", frankenstring);
<<<<<<< HEAD
		
				write_file(atoi(splitInput[1]), frankenstring, strlen(frankenstring));
=======

				write_file(of.fd, frankenstring, strlen(frankenstring));
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea

				k = 0;
			}
			else
			{
				printf("text: %s\n", splitInput[2]);

<<<<<<< HEAD
				write_file(atoi(splitInput[1]), splitInput[2], strlen(splitInput[2]));
=======
				write_file(of.fd, splitInput[2], strlen(splitInput[2]));
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
			}

		}
		else if (!strcmp(splitInput[0], "read") && splitInput[1] && splitInput[2])
		{
<<<<<<< HEAD
			read_file(atoi(splitInput[1]), output2, atoi(splitInput[2]));
=======
			read_file(of.fd, output2, atoi(splitInput[2]));
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
			printf("\n%s\n", output2);
		}
		else if (!strcmp(splitInput[0], "lseek") && splitInput[1] && splitInput[2])
		{
<<<<<<< HEAD

			fs_lseek(atoi(splitInput[1]), atoi(splitInput[2]));

		}
		else if (!strcmp(splitInput[0], "chmod") && splitInput[1] && splitInput[2])
		{
			fs_chmod(splitInput[1], splitInput[2]);
=======
			if (of.name != NULL)
			{
				fs_lseek(of.fd, atoi(splitInput[2]));
			}
			else
			{
				printf("File is not open.");
			}
		}
		else if (!strcmp(splitInput[0], "chmod") && splitInput[1] && splitInput[2])
		{
			fs_chmod(atoi(splitInput[1]), splitInput[2]);
>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea
		}
		else if (!strcmp(splitInput[0], "touch") && splitInput[1])
		{
			fs_touch(splitInput[1]);
		}
		else if (!strcmp(splitInput[0], "cp") && splitInput[1] && splitInput[2])
		{
			fs_cp(splitInput[1], splitInput[2]);
		}
		else if (!strcmp(splitInput[0], "cat") && splitInput[1])
		{
			fs_cat(splitInput[1]);
		}
		else if (!strcmp(splitInput[0], "mv") && splitInput[1] && splitInput[2])
		{
			fs_mv(splitInput[1], splitInput[2]);
		}
		else if(!strcmp(splitInput[0], "rm") && splitInput[1])
		{
			fs_unlink(splitInput[1]);
		}
		else if (!strcmp(splitInput[0], "exit\n"))
		{
			break;
		}
		else
		{
<<<<<<< HEAD
			printf ("invalid command: |%s|\n", splitInput[0]);
		}
	}
}
/*
*/
=======
			printf ("invalid command\n");
		}
	}
}


>>>>>>> 2162979a461162be0f172d9c12420ac6b24e73ea

