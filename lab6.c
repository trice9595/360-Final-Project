
#include "fs_level1.c"

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



int main(int argc, char* argv[])
{
	ip = malloc(sizeof(INODE));
	struct stat mystat;	
    
	char path[256];

	init();
    mount_root();
	
  	printf("Enter string for ls pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");
	ls(path);

	printf("Enter string for cd pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");
	cd(path);

  	printf("Enter string for ls pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");
	ls(path);

	printf("Enter string for mkdir pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");

	mkdir_fs(path);

  	printf("Enter string for ls pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");
	ls(path);

	printf("Enter string for cd pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");
	cd(path);

  	printf("Enter string for ls pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");
	ls(path);

	printf("Enter string for mkdir pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");

	mkdir_fs(path);

  	printf("Enter string for ls pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");
	ls(path);



	printf("Enter string for stat pathname: ");
	fgets(path, sizeof(path), stdin);
	printf("\n");

	stat(path, &mystat);

	return 1;
}
