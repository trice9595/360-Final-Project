#include "type.h"
#include "global.h"


off_t lseek(int fd, off_t offset, int whence);
ssize_t read(int fd, void* buf, size_t nbyte);
//int write(int fd, void* buf, size_t nbyte);

int get_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, buf, BLKSIZE);
}

int put_block(int fd, int blk, char buf[ ])
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, buf, BLKSIZE);
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

int read_group_desc_block(int fd)
{
	get_block(fd, 2, buf);
	gp = (GD *)buf;

	return gp->bg_inode_table;
}

//sets global offset and blk variables
void mailmans_algorithm(int fd, int ino)
{	
	if(inodes_per_block == 0 || inodes_begin_block == 0)
		calculate_inode_block_info(fd);		

	//find block and offset within block
	blk = (ino - 1)/inodes_per_block + 		      
	inodes_begin_block;
	offset = (ino - 1)%inodes_per_block;

}

void calculate_inode_block_info(int fd)
{
		inodes_begin_block = read_group_desc_block(fd);
		inodes_per_block = read_super_block(fd);
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
	printf("dp->inode: %d\n\n", dp->inode);
}


int getino(char* pathname)
{
	
	int ino;
	int sum_rec_len = 0;
	int i_size = 0;
	int x = 0;


	ip = running->cwd;
	print_inode();
	i_size = ip->i_size;
	printf("tokenizing pathname..\n");
	char** names = tokenize(pathname);

	while(names[x] != NULL && strcmp(names[x], "") != 0)
	{

		printf("searching for names[%d] = %s\n", x, names[x]);
		int i = 0;
		for(i = 0; i < 12; i++)
		{
			sum_rec_len = 0;
			printf("searching i_block[%d]...\n\n", i);
			print_inode();
			if(ip->i_block[i] != 0)
			{
				get_block(dev, ip->i_block[i], buf);
				dp = (DIR *)buf;
				sum_rec_len += dp->rec_len;
				while(sum_rec_len < i_size && 
				strcmp(names[x], dp->name) != 0)
				{
					print_dir();
					dp = (DIR *)((char *)dp + dp->rec_len);
					sum_rec_len += dp->rec_len;
				}

				printf("searched iblock[%d]\n", i);

				if(strcmp(names[x], dp->name) ==  0)
				{
					if(dp->name != NULL && 
					dp->file_type != EXT2_FT_REG_FILE)
					{
						printf("DIRECTORY %s FOUND\n", dp->name);
						ino = dp->inode;
						mailmans_algorithm(dev, ino);
		
						get_block(dev, blk, buf);

						ip = (INODE *)buf + offset;
						break;	
					}
					else if(dp->name != NULL && 
					dp->file_type == EXT2_FT_REG_FILE)
					{
						printf("FILE %s FOUND\n", dp->name);
						ino = dp->inode;
						break;
					}
				}else
				{
					printf("ERROR: INVALID PATH\n");
				}
						
			}	

		}
		x++;
	}

	return ino;
}


int search(MINODE *mip, char *name, int fd)
{
   int i; 
   char *cp, sbuf[BLKSIZE];
   DIR *dp;
   INODE *ip;

   ip = &(mip->inode);
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (ip->i_block[i] == 0)
          return 0;

       get_block(fd, ip->i_block[i], sbuf);
       dp = (DIR *)sbuf;
       cp = sbuf;
       while (cp < sbuf + BLKSIZE){
          // print dp->inode, dp->rec_len, dp->name_len, dp->name);
	      printf("dp->inode: %d\n", dp->inode);
	      printf("dp->rec_len: %d\n", dp->rec_len);
	      printf("dp->name_len: %d\n", dp->name_len);
	      printf("dp->name: %s\n", dp->name);

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

MINODE* iget(int fd, int ino)
{
	MINODE* mip = NULL;
	int i = 0;
	printf("searching for minode with inode #%d\n", ino);

	for(i = 0; i < NMINODE; i++)
	{
		//minode exists already
		if(minode[i].ino == ino 
			&& minode[i].dev == fd)
		{
			minode[i].refCount++;
			printf("**minode %d found**\n\n", i);
			return &minode[i];				
		}
		//empty minode found and mip is still null
		else if(minode[i].refCount == 0 && mip == NULL)
		{
			printf("**minode %d created**\n\n", i);
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

	return mip;
}


//STILL NEEDS TESTING
void iput(MINODE *mip)
{
	mip->refCount--;
	if(mip->refCount > 0 && mip->dirty == 1)
	{
		//write back to disk

		mailmans_algorithm(dev, mip->ino);

		get_block(dev, blk, buf);
		ip = (INODE *)buf + offset;
	    
		memcpy(ip, &mip->inode, sizeof(INODE));

		put_block(dev, blk, buf);
		
	}else if(mip->refCount > 0 || mip->dirty == 0)
	{
		return;
	}
	
}





int findmyname(MINODE *parent, int myino, char *myname) 
{
   /*
Given the parent DIR (MINODE pointer) and myino, this function finds 
   the name string of myino in the parent's data block. This is the SAME
   as SEARCH() by myino, then copy its name string into myname[ ].
*/
	

}

int findino(MINODE *mip, int *myino, int *parentino)
{
/*
  For a DIR Minode, extract the inumbers of . and .. 
  Read in 0th data block. The inumbers are in the first two dir entries.
*/
	return 0;
}
