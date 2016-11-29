#include "type.h"
#include "global.h"
#include <time.h>


off_t lseek(int fd, off_t offset, int whence);
ssize_t read(int fd, void* buf, size_t nbyte);
//int write(int fd, void* buf, size_t nbyte);

int get_block(int fd, int blk, char* get_buf)
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, get_buf, BLKSIZE);
}

int get_u32_block(int fd, int blk, u32* get_buf)
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  read(fd, get_buf, BLKSIZE);
}

int put_block(int fd, int blk, char* put_buf)
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, put_buf, BLKSIZE);
}

int put_u32_block(int fd, int blk, u32* put_buf)
{
  lseek(fd, (long)blk*BLKSIZE, 0);
  write(fd, put_buf, BLKSIZE);
}

char** tokenize(char *pathname, char **names)
{
	int i = 0;
	//char** names = malloc(64*sizeof(char *));	
	const char* delim = "/";
	
	names[i] = strtok(pathname, delim);
	while(names[i] != NULL)
	{
		i++;
		names[i] = strtok(NULL, delim);
	}
	names[i - 1] = strtok(names[i - 1], "\n");
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
	char sbuf[1024];
	get_block(fd, 1, sbuf);
	sp = (SUPER *)sbuf;
	
	if (sp->s_magic != 0xEF53)
	{
		printf("NOT an EXT2 FS\n");
    	exit(1);
  	}
	return (1024 << sp->s_log_block_size)/sizeof(INODE);
}

void read_group_desc_block(int fd)
{
	
	char sbuf[1024];
	get_block(fd, 2, sbuf);
	gp = (GD *)sbuf;

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
	printf("ip->i_blocks: %d\n", ip->i_blocks);
	printf("ip->i_mode: %d\n\n", ip->i_mode);
}

void print_access_rights(int permissions)
{
	int i = 0;
	char access[9];
	for(i = 0; i < 9; i++)
		access[i] = '-';

	if(S_IRUSR & permissions)
		access[0] = 'r';

	if(S_IWUSR & permissions)
		access[1] = 'w';

	if(S_IXUSR & permissions)
		access[2] = 'x';

	if(S_IRGRP & permissions)
		access[3] = 'r';

	if(S_IWGRP & permissions)
		access[4] = 'w';

	if(S_IXGRP & permissions)
		access[5] = 'x';

	if(S_IROTH & permissions)
		access[6] = 'r';

	if(S_IWOTH & permissions)
		access[7] = 'w';

	if(S_IXOTH & permissions)
		access[8] = 'x';

	printf("%s", access);
}

int get_num_entries(INODE* inode)
{
	char sbuf[1024];
   int i, num_entries = 0; 
   char *cp;

	ip = inode;
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (ip->i_block[i] == 0)
          return num_entries;

       get_block(dev, ip->i_block[i], sbuf);

       cp = sbuf;
       while (cp < sbuf + BLKSIZE){
          dp = (DIR *)cp;
          cp += dp->rec_len;
		  num_entries++;
       }
   }
   return num_entries;

}

int search_inode(INODE* inode, char *name)
{
   int i; 
   char *cp;
	char sbuf[BLKSIZE];

	ip = inode;	
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (ip->i_block[i] == 0)
          return 0;

       get_block(dev, ip->i_block[i], sbuf);
       dp = (DIR *)sbuf;
       cp = sbuf;
       while (cp < sbuf + BLKSIZE){

			//print_dir();
          //search for name: return its ino if found
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
	char path[64];
	char* names[512] = {NULL};
	int ino = 0;
	int x = 0;
	printf("pathname: %s\n", pathname);
	strcpy(path, pathname);
	//absolute path
	if(path[0] == '/')
	{
		printf("absolute path\n");
		ip = &root->inode;
		file_parent_ino = root->ino;		

		if(strcmp(path, "/") == 0)
		{
			ino = root->ino;
		}
		else
		{
			strcpy(path, &pathname[1]);
			printf("new path: |%s|\n", path);
		}
	}
	//relative path
	else
	{
		ip = &running->cwd->inode;
		file_parent_ino = running->cwd->ino;
	}
		

	if(strcmp(path, ".") != 0)
	{
		tokenize(path, names);
	}
	else
	{
		names[0] = path;
	}

	printf("pathname: |%s|\n", path);

	while(names[x] != NULL && strcmp(names[x], "") != 0)
	{
		printf("searching for names[%d]: |%s|\n", x, names[x]);	
		if(x > 0)
			file_parent_ino = ino;			

		ino = search_inode(ip, names[x]);
		x++;
		if(names[x] != NULL && strcmp(names[x], "") != 0)
		{	
			mailmans_algorithm(*fd, ino);
		
			get_block(*fd, blk, buf);

			ip = (INODE *)buf + offset;
		}

		
	}
	return ino;
}


MINODE* iget(int fd, int ino)
{
	MINODE* mip = NULL;
	int i = 0;

	for(i = 0; i < NMINODE; i++)
	{
		//minode exists already
		if(minode[i].ino == ino 
			&& minode[i].dev == fd)
		{
			minode[i].refCount++;
			return &minode[i];				
		}
		//empty minode found and mip is still null
		else if(minode[i].refCount == 0 && mip == NULL)
		{
			mip = &minode[i];
			break;
		}
	}
	
	mailmans_algorithm(fd, ino);
	get_block(fd, blk, buf2);
	ip = (INODE *)buf2 + offset;
	
	//initialize minode properties
	mip->inode = *ip;
	mip->refCount = 1;
	mip->dev = fd;
	mip->ino = ino;
	mip->dirty = 0;
	mip->mounted = 0;
	mip->mptr = NULL;
	mip->lock = 1;
	

	return mip;
}


//STILL NEEDS TESTING
void iput(MINODE *mip)
{
	mip->refCount--;
	mip->lock = 0;
 	if(mip->refCount == 0 && mip->dirty == 1)
	{
		//write back to disk
		printf("writing minode with ino #%d\n", mip->ino);
		mailmans_algorithm(dev, mip->ino);

		get_block(dev, blk, buf);
		ip = (INODE *)buf + offset;

		memcpy(ip, &mip->inode, sizeof(INODE));
		
		put_block(dev, blk, buf);

	}
}



void print_dir()
{
	char date[128];
	MINODE* mip = iget(dev, dp->inode);

	time_t t = (time_t)mip->inode.i_atime;
	struct tm *tm = localtime(&t);

	strftime(date, sizeof(date) - 1, "%m %d %H:%M", tm);

	if(dp->file_type == 2)
	{
		printf("d");
	}
	else
	{
		printf("-");
	}

	print_access_rights(mip->inode.i_mode);
	printf(" %d", mip->inode.i_links_count);
	printf(" %d", mip->inode.i_size);
	printf(" %s", date);
	printf(" %s\n", dp->name);

	iput(mip);
}


void print_inode_contents()
{
	char sbuf[1024];
   int i; 
   char *cp;
	
   for (i=0; i<12; i++){  // ASSUME DIRs only has 12 direct blocks
       if (ip->i_block[i] == 0)
          return;
		printf("i_block #%d\n", i);
       get_block(dev, ip->i_block[i], sbuf);
       dp = (DIR *)sbuf;
       cp = sbuf;
       while (cp < sbuf + BLKSIZE){
	      //print_dir(dp->inode, dp->name, dp->file_type);
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

int findino(MINODE *mip)
{
	/*For a DIR Minode, extract the inumbers of . and .. 
	Read in 0th data block. The inumbers are in the first two dir 		entries.*/
	char ibuf[BLKSIZE];

	char* cp = ibuf;
	ip = &mip->inode;

	get_block(dev, ip->i_block[0], ibuf);
	dp = (DIR *) cp;

    cp += dp->rec_len;
    dp = (DIR *) cp;

	if(strcmp(dp->name, "..") == 0)
		return dp->inode;

	
	return -1;
}



