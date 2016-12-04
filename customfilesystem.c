//custom file system
//John Howley and Tim Rice
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_level1.c"
#include "fs_level2.c"

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

int mount_root()
{
	dev = open("diskimage", O_RDONLY);
	root = iget(dev, 2);

	printf("root\n");
	printf("dev: %d\n", root->dev);
	printf("ino: %d\n", root->ino);
	printf("refCount: %d\n", root->refCount);
	printf("dirty: %d\n", root->dirty);
	printf("mounted: %d\n", root->mounted);

	proc[0].cwd = iget(root->dev, 2);
	proc[1].cwd = iget(root->dev, 2);	

	running = &proc[0];

	if (!dev && !root && !running)
	{
		return 0;
	}

	return 1;
}
int ls(char* pathname)
{
	int ino;
	int i_size = 0;
	
 	dev = running->cwd->dev;

	MINODE* mip = running->cwd;
	
	if(pathname)
	{
		if(pathname[0] == "/")	
		{	
			dev = root->dev;
		}
		else if(pathname[0] == '\n')
		{
			printf ("No path entered\n");

			pathname[0] = '.';
			pathname[1] = '\0';
		}
		

		printf("getting ino with entered pathname...\n");
		ino = getino(dev, pathname);
		printf("got ino!\n", root->dev);
		

		mip = iget(dev, ino);
	}
	else
	{
		printf ("No path entered\n");

		pathname[0] = '.';
		pathname[1] = '\0';

		printf("getting current ino ...\n");
		ino = getino(dev, pathname);
		printf("got ino!\n", root->dev);
		

		mip = iget(3, ino);
	}

	ip = &mip->inode;
	i_size = ip->i_size;

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
