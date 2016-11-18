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

void print_inode()
{
	printf("ip->i_size: %d\n", ip->i_size);
	printf("ip->i_blocks: %d\n", ip->i_blocks);
	printf("ip->i_ctime: %d\n", ip->i_ctime);
	printf("ip->i_atime: %d\n", ip->i_atime);
}

void print_dir()
{
	printf("dp->name: %s\n", dp->name);
	printf("dp->file_type: %d\n", dp->file_type);
	printf("dp->inode: %d\n", dp->inode);
	printf("dp->rec_len: %d\n\n", dp->rec_len);
}

void print_super_block()
{
	printf("sp->s_inodes_count: %d\n", sp->s_inodes_count);
	printf("sp->s_blocks_count: %d\n", sp->s_blocks_count);
	printf("sp->s_log_block_size: %d\n", sp->s_log_block_size);
	printf("sp->s_first_data_block: %d\n", sp->s_first_data_block);

}

int getino(int fd, char* pathname)
{
	
	int ino, blk, offset;
	int inodes_begin_block, inodes_per_block;
	int x = 0;

	printf("reading super block...\n");
	//read super block
	inodes_per_block = read_super_block(fd);
	

	printf("reading group desc block...\n");
	// read group descriptor block
	inodes_begin_block = read_group_desc_block(fd);

	printf("reading inode...\n", root->dev);

	get_block(fd, inodes_begin_block, buf);
	ip = (INODE *)buf + 1;
	print_inode();

	printf("tokenizing pathname..\n");
	char** names = tokenize(pathname);

	while(names[x] != NULL && strcmp(names[x], "") != 0)
	{

		printf("searching for names[%d] = %s\n", x, names[x]);
		int i = 0;
		for(i = 0; i < 12; i++)
		{
			printf("searching i_block[%d]\n", i);
			if(ip->i_block[i] != 0)
			{
				get_block(fd, ip->i_block[i], buf);
				dp = (DIR *)buf;

				while(strcmp(names[i], dp->name) != 0 
						&& dp->file_type != 8 
						&& dp->file_type != 0)
				{
					dp = (DIR *)((char *)dp + dp->rec_len);	
				}

				if(dp->name != NULL && 
				dp->file_type != EXT2_FT_REG_FILE)
				{
					ino = dp->inode;
					blk = (ino - 1)/inodes_per_block + inodes_begin_block;
					offset = (ino - 1)%inodes_per_block;
					
					get_block(fd, blk, buf);
					ip = (INODE *)buf + offset;
					break;	
				}
				else if(dp->name != NULL && 
				dp->file_type == EXT2_FT_REG_FILE)
				{
					ino = dp->inode;
					break;
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
	INODE cpy;
	int i = 0, blk, offset;
	int inodes_per_block, inodes_begin_block;
	
	for(i = 0; i < NMINODE; i++)
	{
		//minode exists already
		if(minode[i].ino == ino 
			&& minode[i].dev == fd)
		{
			minode[i].refCount++;
			printf("minode %d found\n", i);
			return &minode[i];				
		}
		//empty minode found and mip is still null
		else if(minode[i].refCount == 0 && mip == NULL)
		{
			printf("minode %d created\n", i);
			mip = &minode[i];
		}
	}

	//read super block for inodes per block
	inodes_per_block = read_super_block(fd);

	//read group desc block for inodes begin block
	inodes_begin_block = read_group_desc_block(fd);

	//find block and offset within block
	blk = (ino - 1)/inodes_per_block + 		      
	inodes_begin_block;
	offset = (ino - 1)%inodes_per_block;

	//get inode
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

void iput(MINODE *mip)
{
	mip->refCount--;
	if(mip->refCount > 0 && mip->dirty == 1)
	{
		//write back to disk
		
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
