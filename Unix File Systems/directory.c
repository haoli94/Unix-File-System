#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// remove the placeholder implementation and replace with your own
int directory_findname(struct unixfilesystem *fs, const char *name,
	int dirinumber, struct direntv6 *dirEnt) {
    int dfd = fs->dfd;
    struct inode dir_inode;
    int get_dirinode = inode_iget(fs,dirinumber,&dir_inode);
    //Get the inode of the directory
    if (get_dirinode == -1){
	fprintf(stderr, "Get directory inode failed!\n");
	return -1;
    }
    if((dir_inode.i_mode & IFMT) != IFDIR){
	fprintf(stderr, "It is not a valide directory!\n");
	return -1;
    }
    int dir_size = inode_getsize(&dir_inode);
    //Get the size of the directory inode, which means the number of files inside
    int dirs_per_block = DISKIMG_SECTOR_SIZE / sizeof(struct direntv6);
    //The size of direntv6 = 16
    int block_used = (dir_size) / DISKIMG_SECTOR_SIZE + 1;
    //Get the blocks used in the i_addr
    for (int i=0; i < block_used;i++){
	struct direntv6 buf[dirs_per_block];
	//Construct a direntv6 array
	int bytes_used = file_getblock(fs, dirinumber, i, buf);
	int dirs = bytes_used / sizeof(struct direntv6);
	//Iterate through the blocks inside i_addr to look for the target file
	for(int j=0;j<dirs;j++){
	    if (strcmp(buf[j].d_name,name)==0){
		*dirEnt = buf[j];
		return 0;
		//If the file is found, return 0
	    }
	}

    }
    return -1;
    //If the file has not been found, return -1
}
