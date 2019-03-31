#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

// remove the placeholder implementation and replace with your own
int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    int dfd = fs->dfd;
    size_t inode_size = sizeof(struct inode);
    int inodes_number_per_block = DISKIMG_SECTOR_SIZE / inode_size;
    inumber -= 1;
    //The inode starts from 1
    int sectorNum = inumber / inodes_number_per_block;
    //Calculate the sector offset
    int inode_num_remain = inumber % inodes_number_per_block;
    //Calculate the offset inside a specific sector
    struct inode buf[inodes_number_per_block];
    //Create a inode array to store the inodes read from disk sector
    if(diskimg_readsector(dfd, INODE_START_SECTOR + sectorNum, buf)){
	*inp = buf[inode_num_remain];
	//Get the inode info from the sector read from disk
    }else{
	fprintf(stderr, "Unable to read the disk!\n");
	return -1;
    }
    return 0;  
}

// remove the placeholder implementation and replace with your own
int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    int dfd = fs->dfd;
    if ((inp->i_mode & ILARG) == 0){
	//If the current file is a small file, all of its i_addr are direct payload
	return inp->i_addr[blockNum];
    }else{
	//Depends on different file size
	int singly_num = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
	if(singly_num * 7 > blockNum){
	    //In this case it only uses singly-indirect payloads
	    int singly_offset = blockNum / singly_num;
	    //Calculate the i_addr offset
	    int singly_remain = blockNum % singly_num;
	    //Calculate the block offset inside a i_addr
	    int singly_block = inp->i_addr[singly_offset];
	    uint16_t buf[singly_num];
	    if(diskimg_readsector(dfd, singly_block, buf)){
		return buf[singly_remain];
		//read the specific block
	    }else{
		fprintf(stderr, "Unable to read the disk!\n");
		return -1;
	    }
	}else{
	    //In this case, it uses doubly-indirect payloads
	    int singly_num = DISKIMG_SECTOR_SIZE / sizeof(uint16_t);
	    int blocks_in_double = blockNum - singly_num * 7;
	    int offset1 = blocks_in_double/singly_num;
	    //We need to calculate the offset in the first indirect block first
	    uint16_t buf1[singly_num];
	    if(!diskimg_readsector(dfd, inp->i_addr[7], buf1)){
		fprintf(stderr, "Unable to read the disk!\n");
		return -1;        
	    }
	    //Fetch the first indirect address block stored in disk
	    int offset2 = blocks_in_double % singly_num;
	    //We need to calculate the offset in the second indirect block
	    uint16_t buf2[singly_num];
	    if(!diskimg_readsector(dfd, buf1[offset1], buf2)){
		fprintf(stderr, "Unable to read the disk!\n");
		return -1;
	    }
	    //Fetch the second indirect address block stored in disk
	    return buf2[offset2];
	    //return the payload address
	}
    }
}

int inode_getsize(struct inode *inp) {
    return ((inp->i_size0 << 16) | inp->i_size1); 
}
