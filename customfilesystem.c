//custom file system
//John Howley and Tim Rice
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_level1.c"

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

		i = 1;

		splitInput[0] = strtok(input, " ");

		while (splitInput[i] = strtok(NULL, " "))
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
			mkdir(splitInput[1]);
		}
		else if (splitInput[1] && !strcmp(splitInput[0], "rmdir"))
		{
			rmdir(splitInput[1]);
		}
		else if (splitInput1 && !strcmp(splitInput[0], "creat"))
		{
			creat(splitInput[1]);
		}
		else if (splitInput[1] && splitInput [2] && !strcmp(splitInput[0], "symlink"))
		{
			symlink(splitInput[1], splitInput[2]);
		}
		else if (!strcmp(splitInput[0], "readlink"))
		{
			readlink();
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
