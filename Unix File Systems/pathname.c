#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
	if (strcmp(pathname, "/")==0) {
		return ROOT_INUMBER;
	}
	//If the path that we are going to search is the current director, return the current ROOT_INUMBER. 
	char* start = strdup(pathname);
	start++;
	//Move the pointer to the pathname ahead by one
	int dirnum = ROOT_INUMBER;
	const char s[2] = "/";
	char *token;
	token = strtok(start, s);
	//Utilize strtok to parse the directory
	struct direntv6 dirent;
	while (token != NULL){
		int length = strlen(token);
                char subdir[16] = {'\0'};
                strncpy(subdir, token, length);
		//Copy the current path out for search
                if (directory_findname(fs, subdir, dirnum, &dirent) == -1) return -1;
                token = strtok(NULL, s);
                dirnum = dirent.d_inumber;
		//Move into the subdirectory
	}
	return dirent.d_inumber;
	//If found, return the route_number.

}

