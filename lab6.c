#include "util.c"




void init()
{
	int i = 0;

	proc[0].uid = 0;
	proc[1].uid = 1;
	proc[0].cwd = 0;
	proc[1].cwd = 0;

	root = 0;

	for(i = 0; i < NMINODE; i++)
	{
		minode[i].refCount = 0;
	}

}

void mount_root()
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
}

void ls(char* pathname)
{

	int ino;
	
 	dev = running->cwd->dev;

	MINODE* mip = running->cwd;
	
	printf("running->cwd->dev: %d\n", running->cwd->dev);
	if(pathname)
	{
		if(pathname[0] == "/")		
			dev = root->dev;

		printf("dev: %d\n", dev);
		ino = getino(dev, pathname);
		printf("root->dev: %d\n", root->dev);
		

		/*printf("dev: %d\n", dev);
		printf("root->dev: %d\n", root->dev);
*/
		mip = iget(3, ino);
	}

	ip = &mip->inode;

	int i = 0;
	for(i = 0; i < 12; i++)
	{
		
		if(ip->i_block[i] != 0)
		{
			get_block(3, ip->i_block[i], buf);
			dp = (DIR *)buf;
			while(dp->file_type != 8 && dp->file_type != 0)
			{
				print_dir();
				dp = (DIR *)((char *)dp + dp->rec_len);
			}
			
			if(dp->file_type == 0)
				break;
		}
	}
}

void cd(char* pathname)
{
	printf("here\n");
	if(pathname == NULL || strcmp(pathname, "/"))
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
