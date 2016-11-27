#include "type.h"
#include "global.h"


off_t lseek(int fd, off_t offset, int whence);
ssize_t read(int fd, void* buf, size_t nbyte);
//int write(int fd, void* buf, size_t nbyte);

int get_block(int fd, int blk, char* get_buf)
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, get_buf, BLKSIZE);
}


int put_block(int fd, int blk, char* put_buf)
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, put_buf, BLKSIZE);
}

char** tokenize(char *pathname)
{
	int i = 0;
	char** names = malloc(64*sizeof(char *));	
	const char* delim = "/";

	names[i] = strtok(pathname, delim);
	while(names[i] != NULL)
	{
		i++;
		names[i] = strtok(NULL, delim);
	}
	names[i - 1] = strtok(names[i - 1], "\n");
	return names;
}

void read_path(char* basename, char** path_parts)
{
	int i = 0;

	//if path is absolute
	if(path_parts[0] == "/")
		dev = root->dev;
	else
		dev = running->cwd->dev;


	while(path_parts[i + 1] != NULL)
	{
		i++;
	}

	strcpy(basename, path_parts[i]);	
	printf("basename: %s\n", basename);
	path_parts[i] = NULL;


}

//returns inodes_per_block
int read_super_block(int fd)
{

	get_block(fd, 1, buf);
	sp = (SUPER *)buf;
	
	if (sp->s_magic != 0xEF53)
	{
		printf("NOT an EXT2 FS\n");
    	exit(1);
  	}
	return (1024 << sp->s_log_block_size)/sizeof(INODE);
}

void read_group_desc_block(int fd)
{
	get_block(fd, 2, buf);
	gp = (GD *)buf;

	imap =gp->bg_inode_bitmap;
	bmap = gp->bg_block_bitmap;
	ninodes = gp->bg_free_inodes_count;
	nblocks = gp->bg_free_blocks_count;
	inodes_begin_block = gp->bg_inode_table;
}


void calculate_mount_info(int fd)
{
	read_group_desc_block(fd);
	inodes_per_block = read_super_block(fd);
}


//sets global offset and blk variables
void mailmans_algorithm(int fd, int ino)
{	
	if(inodes_per_block == 0 || inodes_begin_block == 0)
		calculate_mount_info(fd);		

	//find block and offset within block
	blk = (ino - 1)/inodes_per_block + 		      
	inodes_begin_block;
	offset = (ino - 1)%inodes_per_block;

}

void print_inode()
{
	printf("printing inode...\n");
	printf("ip->i_size: %d\n", ip->i_size);
	printf("ip->i_blocks: %d\n\n", ip->i_blocks);
}

void print_dir()
{
	printf("dp->name: %s\n", dp->name);
	printf("dp->file_type: %d\n", dp->file_type);
	printf("dp->inode: %d\n", dp->inode);
	printf("dp->name_len: %d\n", dp->name_len);
	printf("dp->rec_len: %d\n", dp->rec_len);
	printf("sizeof(dp): %d\n\n", sizeof(dp));
}

void print_inode_contents()
{
   int i; 
   char *cp;
	
	printf("printing inode contents...\n");
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (ip->i_block[i] == 0)
          return;
		printf("i_block #%d\n", i);
       get_block(dev, ip->i_block[i], buf);
       dp = (DIR *)buf;
       cp = buf;
       while (cp < buf + BLKSIZE){
	      print_dir();
		  if(dp->rec_len <= 0)
		  {
			printf("ERROR: dp->rec_len of %d is invalid\n", dp->rec_len);
			break;
	      }
          cp += dp->rec_len;
          dp = (DIR *)cp;
       }
   }

}



int get_num_entries(INODE* inode)
{
   int i, num_entries = 0; 
   char *cp;

	ip = inode;	
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (ip->i_block[i] == 0)
          return 0;

       get_block(dev, ip->i_block[i], buf);
       dp = (DIR *)buf;
       cp = buf;
		num_entries = 1;
       while (cp < buf + BLKSIZE){
          cp += dp->rec_len;
          dp = (DIR *)cp;
			num_entries++;
       }
   }
   return num_entries;

}

int search_inode(INODE* inode, char *name)
{
   int i; 
   char *cp;

	ip = inode;	
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (ip->i_block[i] == 0)
          return 0;

       get_block(dev, ip->i_block[i], buf);
       dp = (DIR *)buf;
       cp = buf;
       while (cp < buf + BLKSIZE){
          // print dp->inode, dp->rec_len, dp->name_len, dp->name);
	      printf("\ndp->inode: %d\n", dp->inode);
	      printf("dp->rec_len: %d\n", dp->rec_len);
	      printf("dp->name_len: %d\n", dp->name_len);
	      printf("dp->name: %s\n\n", dp->name);

          // WRITE YOUR CODE TO search for name: return its ino if found
		  if(strcmp(dp->name, name) == 0)
		  {
			  return dp->inode;
		  }
          cp += dp->rec_len;
          dp = (DIR *)cp;
       }
   }
   return 0;

}


int getino(int* fd, char* pathname)
{
	
	int ino = 0;
	int sum_rec_len = 0;
	int i_size = 0;
	int x = 0;


	ip = &running->cwd->inode;
	print_inode();
	i_size = ip->i_size;
	printf("tokenizing pathname..\n");
	char** names = tokenize(pathname);

	while(names[x] != NULL && strcmp(names[x], "") != 0)
	{

		printf("searching for names[%d] = %s\n", x, names[x]);
		int i = 0;
		ino = search_inode(ip, names[x]);

		x++;
		if(names[x] != NULL && strcmp(names[x], "") != 0)
		{	
			
			mailmans_algorithm(*fd, ino);
		
			get_block(*fd, blk, buf);

			ip = (INODE *)buf + offset;
		}
	}
	
	printf("got ino #%d\n", ino);
	return ino;
}


MINODE* iget(int fd, int ino)
{
	MINODE* mip = NULL;
	char buffer[1024];
	int i = 0;
	printf("searching for minode with inode #%d\n", ino);

	for(i = 0; i < NMINODE; i++)
	{
		//minode exists already
		if(minode[i].ino == ino 
			&& minode[i].dev == fd)
		{
			minode[i].refCount++;
			printf("**minode found at 0**\n\n", i);
			return &minode[i];				
		}
		//empty minode found and mip is still null
		else if(minode[i].refCount == 0 && mip == NULL)
		{
			printf("**minode created at 0**\n\n", i);
			mip = &minode[i];
		}
	}

	mailmans_algorithm(fd, ino);

	get_block(fd, blk, buf);
	ip = (INODE *)buf + offset;
	
	//initialize minode properties
	mip->inode = *ip;
	
	mip->refCount = 1;
	mip->dev = fd;
	mip->ino = ino;
	mip->dirty = 0;
	mip->mounted = 0;
	mip->mptr = NULL;
	
	print_inode_contents();

	return mip;
}


//STILL NEEDS TESTING
void iput(MINODE *mip)
{
	mip->refCount--;
	if(mip->refCount > 0 && mip->dirty == 1)
	{
		//write back to disk
		printf("writing back to disk at ino #%d...\n", mip->ino);
		mailmans_algorithm(dev, mip->ino);

		get_block(dev, blk, buf);
		ip = (INODE *)buf;

		printf("sizeof(INODE): %d\n", sizeof(INODE));	    

		memcpy(ip, &mip->inode, sizeof(INODE));
		
		put_block(dev, blk, buf);
		
	}else if(mip->refCount > 0 || mip->dirty == 0)
	{
		return;
	}
	
}


int findmyname(MINODE *parent, int myino, char *myname) 
{
	/*Given the parent DIR (MINODE pointer) and myino, this function finds the name string of myino in the parent's data block. This is the SAME as SEARCH() by myino, then copy its name string into myname[ ].*/

   int i; 
   char *cp;

	ip = &parent->inode;
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (ip->i_block[i] == 0)
          return 0;

       get_block(dev, ip->i_block[i], buf);
       dp = (DIR *)buf;
       cp = buf;
       while (cp < buf + BLKSIZE){
          // print dp->inode, dp->rec_len, dp->name_len, dp->name);
	      printf("\ndp->inode: %d\n", dp->inode);
	      printf("dp->rec_len: %d\n", dp->rec_len);
	      printf("dp->name_len: %d\n", dp->name_len);
	      printf("dp->name: %s\n\n", dp->name);

          // WRITE YOUR CODE TO search for name: return its ino if found
		  if(dp->inode == myino)
		  {
			  myname = dp->name;
			  return 1;
		  }
          cp += dp->rec_len;
          dp = (DIR *)cp;
       }
	}
}

int findino(MINODE *mip, int *myino, int *parentino)
{
	/*For a DIR Minode, extract the inumbers of . and .. 
	Read in 0th data block. The inumbers are in the first two dir 		entries.*/

	char* cp;
	ip = &mip->inode;

	get_block(dev, ip->i_block[0], buf);
	dp = (DIR *) buf;
	
	printf("myino: %d", dp->inode);
	myino = &dp->inode;
	

    cp += dp->rec_len;
    dp = (DIR *)cp;
	
	printf("parentino: %d", dp->inode);
	parentino = &dp->inode;

	return 1;
}



