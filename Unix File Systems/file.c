#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "inode.h"
#include "diskimg.h"

// remove the placeholder implementation and replace with your own
int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    int dfd = fs->dfd;
    struct inode inp;
    int get_inode = inode_iget(fs,inumber,&inp);
    //Get the inode of the file
    if (get_inode == -1){
	fprintf(stderr, "Get inode failed!\n");
	return -1;
    }
    int block_address = inode_indexlookup(fs, &inp, blockNum);
    //Get the block address of a specific block inside a file  
    int file_size = inode_getsize(&inp);
    int block_num = file_size / DISKIMG_SECTOR_SIZE;
    int get_block;
    if (blockNum<block_num){
	get_block = DISKIMG_SECTOR_SIZE;
	//If the block is not the last payload block return 512 Bytes
    }else{
	get_block = file_size - DISKIMG_SECTOR_SIZE*block_num;
	//If the block is the last payload block return the remaining bytes
    }

    if(!diskimg_readsector(dfd,block_address, buf)){
	fprintf(stderr, "Unable to read the file block!\n");
	return -1;
    }
	//Read the block into buf
    return get_block;  
}
