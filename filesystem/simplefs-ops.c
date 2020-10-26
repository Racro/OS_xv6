#include "simplefs-ops.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int simplefs_create(char *filename){
    /*
	    Create file with name `filename` from disk
	*/
	_Bool check=0;;
	for (int i=0;i<MAX_FILES;i++){
		struct inode_t inodeptr;
		simplefs_readInode(i, &inodeptr);
		if (inodeptr.status == INODE_IN_USE){
			if (strcmp(inodeptr.name, filename) == 0){
				return -1;
			}
		}
		else{
			check=1;
		}
	}
	if (check==1){
		int num = simplefs_allocInode();
		if (num==-1){
			return -1;
		}
		else{
			struct inode_t inodeptr;
			inodeptr.status = INODE_IN_USE;
			strcpy(inodeptr.name, filename);
			inodeptr.file_size=0;
			for (int j=0;j<MAX_FILE_SIZE;j++){
				inodeptr.direct_blocks[j]=-1;
			}
			simplefs_writeInode(num,&inodeptr);
			return num;
		}
	}
	else{
		return -1;
	}
}


void simplefs_delete(char *filename){
    /*
	    delete file with name `filename` from disk
	*/
	for (int i=0;i<MAX_FILES;i++){
		struct inode_t inodeptr;
		simplefs_readInode(i, &inodeptr);
		if (inodeptr.status == INODE_IN_USE){
			if (strcmp(inodeptr.name, filename) == 0){
				for (int j=0;j<MAX_FILE_SIZE;j++){
					if (inodeptr.direct_blocks[j]!=-1){
						simplefs_freeDataBlock(inodeptr.direct_blocks[j]);
					}
				}
				simplefs_freeInode(i);
				return;
			}
		}
	}
}

int simplefs_open(char *filename){
    /*
	    open file with name `filename`
	*/
	for (int i=0;i<MAX_FILES;i++){
		struct inode_t inodeptr;
		simplefs_readInode(i, &inodeptr);
		if (inodeptr.status == INODE_IN_USE){
			if (strcmp(inodeptr.name, filename) == 0){
				for (int j=0;j<MAX_OPEN_FILES;j++){
					if (file_handle_array[j].inode_number == -1){
						file_handle_array[j].offset=0;
						file_handle_array[j].inode_number=i;
						return j;						
					}
				}

			}
		}
	}	
    return -1;
}

void simplefs_close(int file_handle){
    /*
	    close file pointed by `file_handle`
	*/
	file_handle_array[file_handle].offset=0;
	file_handle_array[file_handle].inode_number=-1;
}

int simplefs_read(int file_handle, char *buf, int nbytes){
    /*
	    read `nbytes` of data into `buf` from file pointed by `file_handle` starting at current offset
	*/
	int off=file_handle_array[file_handle].offset;
	int inode_num=file_handle_array[file_handle].inode_number;

	struct inode_t inodeptr;
	simplefs_readInode(inode_num, &inodeptr);

	if((off+nbytes) > 256){
		return -1;
	}
	if(nbytes > inodeptr.file_size){
		return -1;
	}

	char* dum = (char *) malloc(257);
	dum=strcpy(dum,"");
	for (int j=0;j<MAX_FILE_SIZE;j++){
		if (inodeptr.direct_blocks[j] != -1){
			char* b = (char *) malloc(65);
			simplefs_readDataBlock(inodeptr.direct_blocks[j],b);
			strcat(dum,b);

//			printf("%s\n", b);
		}
	}
	int count=0;
	for (int i=off;i<off+nbytes;i++){
		buf[count]=dum[i];
		count=count+1;
	}
    return 0;
}


int simplefs_write(int file_handle, char *buf, int nbytes){
    /*
	    write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset
	*/
	int off=file_handle_array[file_handle].offset;
	int inode_num=file_handle_array[file_handle].inode_number;

	struct inode_t inodeptr;
	simplefs_readInode(inode_num, &inodeptr);
	

	if((off+nbytes) > 256){
		return -1;
	}

	char* b1 = (char *) malloc(65);
	char* b2 = (char *) malloc(65);
	char* b3 = (char *) malloc(65);
	char* b4 = (char *) malloc(65);

	int pages=0;
	for (int j=0;j<MAX_FILE_SIZE;j++){
		if (inodeptr.direct_blocks[j] != -1){
			pages=pages+1;
			if (pages==1){
				simplefs_readDataBlock(inodeptr.direct_blocks[j],b1);
			}
			if (pages==2){
				simplefs_readDataBlock(inodeptr.direct_blocks[j],b2);
			}
			if (pages==3){
				simplefs_readDataBlock(inodeptr.direct_blocks[j],b3);
			}
			if (pages==4){
				simplefs_readDataBlock(inodeptr.direct_blocks[j],b4);
			}
		}
	}
	int count=0;
	if (nbytes>0 && off<64){
		if (pages==0){
			int block_num=simplefs_allocDataBlock();
			if (block_num == -1){
				return -1;
			}
			inodeptr.direct_blocks[0]=block_num;
			pages=1;
		}
		while (off<64 && nbytes>0){
			b1[off]=buf[count];
			count=count+1;
			nbytes=nbytes-1;
			off=off+1;
		}
//		printf("%s\n", buf);
		simplefs_writeDataBlock(inodeptr.direct_blocks[0],b1);
	}

	if (nbytes>0 && off<128){
//		printf( " pages %d\n", pages);
		if (pages<=1){
			int block_num=simplefs_allocDataBlock();
			if (block_num == -1){
				return -1;
			}
			inodeptr.direct_blocks[1]=block_num;
			pages=2;
		}
		while (off<128 && nbytes>0){
//			printf( "pages %s\n", b2);
			b2[off-64]=buf[count];
			count=count+1;
			nbytes=nbytes-1;
			off=off+1;
		}
		simplefs_writeDataBlock(inodeptr.direct_blocks[1],b2);		
	}

	if (nbytes>0 && off<192){
		if (pages<=2){
			int block_num=simplefs_allocDataBlock();
			if (block_num == -1){
				return -1;
			}
			inodeptr.direct_blocks[2]=block_num;
			pages=3;
		}
		while (off<192 && nbytes>0){
			b3[off-128]=buf[count];
			count=count+1;
			nbytes=nbytes-1;
			off=off+1;
		}
		simplefs_writeDataBlock(inodeptr.direct_blocks[2],b3);		
	}

	if (nbytes>0 && off<256){
		if (pages<=3){
			int block_num=simplefs_allocDataBlock();
//			printf("block_num %d\n", block_num);
			if (block_num == -1){
				return -1;
			}
			inodeptr.direct_blocks[3]=block_num;
			pages=4;
		}
		while (off<256 && nbytes>0){
			b4[off-192]=buf[count];
			count=count+1;
			nbytes=nbytes-1;
			off=off+1;
		}
		simplefs_writeDataBlock(inodeptr.direct_blocks[3],b4);		
	}
	if (off+nbytes > inodeptr.file_size){
		inodeptr.file_size=off+nbytes;
	}
	simplefs_writeInode(inode_num,&inodeptr);

    return 0;
}


int simplefs_seek(int file_handle, int nseek){
    /*
	   increase `file_handle` offset by `nseek`
	*/
	int off = file_handle_array[file_handle].offset;
	off=off+nseek;
	if (off<0){
		return -1;
	}
	else if (off > 256){
		return -1;
	}
//	printf("offset %d\n", off);
    file_handle_array[file_handle].offset=file_handle_array[file_handle].offset+nseek;
    return 0;

}