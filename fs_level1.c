#include "util.c"


void ls(char* pathname)
{

	int ino;
	int i_size = 0;
	
 	dev = running->cwd->dev;

	MINODE* mip = running->cwd;
	
	if(pathname[0] != '/' && pathname[0] != '\n' )
	{
		/*if(pathname[0] == "/" || pathname[0] == '\n')	
		{	
			printf("ls for cwd\n");
			pathname[0] = '.';
			pathname[1] = '\0';
		}*/

		ino = getino(pathname);
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
		ino = getino(pathname);
		running->cwd = iget(dev, ino);
	}
}

void mkdir(char* pathname)
{
	int ino;
	if(pathname == NULL || strcmp(pathname, "/") == 0)
	{
		ino = root->ino;
	}
	else
	{
		ino = getino(pathname);
	}
	
	MINODE* mip = iget(dev, ino);

	//traverse mip inode until space is found to mkdir

}
