#include "alloc.h"

/* Code to allocate page of 4KB size with mmap() call and
* initialization of other necessary data structures.
* return 0 on success and 1 on failure (e.g if mmap() fails)
*/
int init()
{
	// Write your code below
	for (int i=0;i<512;i++){
		freelist[i]=0;
	}

	region = mmap(
    (void*) 0,   // Map from the start of the 0th page
    PAGESIZE,                         // for one page length
    PROT_READ|PROT_WRITE,
    MAP_PRIVATE|MAP_ANON,             // to a shared block of hardware memory
    -1, 0
  	);

  	if (region == MAP_FAILED) {
    	perror("Could not mmap");
    	return 1;
  	}

  	counter=1;
  	start_addr=region;

  	return 0;
}

/* optional cleanup with munmap() call
* return 0 on success and 1 on failure (if munmap() fails)
*/
int cleanup()
{

	// Write your code below
	int unmap_result = munmap(region, PAGESIZE);
  	if (unmap_result != 0) {
    	perror("Could not munmap");
    	return 1;
  	}

  	for (int i=0;i<512;i++){
  		freelist[i]=0;
  	}
  // getpagesize
  	return 0;
  
}

/* Function to allocate memory of given size
* argument: bufSize - size of the buffer
* return value: on success - returns pointer to starting address of allocated memory
*               on failure (not able to allocate) - returns NULL
*/
char *alloc(int bufSize)
{
	// write your code below
	if (bufSize%8 != 0){
		return NULL;
	}
	int a=bufSize/8;
	int i;
	_Bool check=1;
	for (i=0;i<512;i++){
		if (freelist[i]!=0){
			a=bufSize/8;
		}
		else if (freelist[i]==0 && a <= bufSize/8){
			a=a-1;
		}

		if (a==0){
			break;
		}
	}
	if (i==512){
		return NULL;
	}
	a=bufSize/8;
  	i=i-a+1;
  	for (int j=i;j<i+a;j++){
  		freelist[j]=counter;
  	}
  	
  	counter=counter+1;
  	return (region+(8*i));
}


/* Function to free the memory
* argument: takes the starting address of an allocated buffer
*/
void dealloc(char *memAddr)
{
	// write your code below
	int offset=(int)(memAddr-start_addr);
//	printf("%d",offset);
	offset=offset/8;
	int a=freelist[offset];
	while(freelist[offset]==a){
		freelist[offset]=0;
		offset=offset+1;
	}
}
