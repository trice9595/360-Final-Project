#include "util.c"


SUPER *sp = NULL;
GD    *gp = NULL;
INODE *ip = NULL;
DIR   *dp = NULL;

MINODE minode[NMINODE] = {0};
PROC   proc[NPROC] = {0}, *running = NULL;
MINODE *root = NULL;

int dev = 0;
char buf[256] = {0};

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
	dev = open("diskimage", O_RDONLY);
	root = iget(dev, 2);

	printf("dev: %d\n", dev);

	printf("root\n");
	printf("dev: %d\n", root->dev);
	printf("ino: %d\n", root->ino);
	printf("refCount: %d\n", root->refCount);
	printf("dirty: %d\n", root->dirty);
	printf("mounted: %d\n", root->mounted);

	//set processes current working directory to root minode
	proc[0].cwd = iget(root->dev, 2);
	proc[1].cwd = iget(root->dev, 2);	

	//set running process to first process
	running = &proc[0];
}

void ls(char* pathname)
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

void cd(char* pathname)
{
	printf("here\n");
	if(pathname == NULL || strcmp(pathname, "/") == 0)
	{
		printf("here1\n");
		running->cwd = root;
	}
	else
	{

		printf("here2\n");
		running->cwd = getino(3, pathname);
	}
}

int main(int argc, char* argv[])
{
	ip = malloc(sizeof(INODE));
	struct stat mystat;	
    
	char ls_path[256];
	char cd_path[256];	
	char stat_path[256];

	init();
    mount_root();
	
  	printf("Enter string for ls pathname: ");
	fgets(ls_path, sizeof(ls_path), stdin);
	printf("\n");

	ls(ls_path);

	printf("Enter string for cd pathname: ");
	fgets(cd_path, sizeof(cd_path), stdin);
	printf("\n");

	cd(cd_path);

	printf("Enter string for stat pathname: ");
	fgets(stat_path, sizeof(stat_path), stdin);
	printf("\n");

	stat(stat_path, &mystat);

	return 1;
}
