#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>

int item_to_produce;
int total_items, max_buf_size, num_workers;
// declare any global data structures, variables, etc that are required
// e.g buffer to store items, pthread variables, etc
int* buf=NULL;
int item_in_buf;
int count=0;

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_worker=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_master=PTHREAD_COND_INITIALIZER;

void print_produced(int num) {

  printf("Produced %d\n", num);
}

void print_consumed(int num, int worker) {
  count=count+1;
  printf("Consumed %d by worker %d\n", num, worker);
  
}


/* produce items and place in buffer (array or linked list)
 * add more code here to add items to the buffer (these items will be consumed
 * by worker threads)
 * use locks and condvars suitably
 */
void *generate_requests_loop(void *data)
{
//	printf("hey %d",1);
  int thread_id = *((int *)data);
  pthread_mutex_lock(&lock);
  int j=0;
  while(1)
    {
  //    pthread_cond_signal(&cond_master);
      if(item_to_produce >= total_items)
		break;
	  if (item_in_buf == max_buf_size){
      pthread_cond_signal(&cond_worker);
	  	pthread_cond_wait(&cond_master,&lock);
      
	  }
    else{
      for (int i=0;i<max_buf_size;i++){
      	if(buf[i] == -1){
      		buf[i]=item_to_produce;
      		item_in_buf=item_in_buf+1;
      		break;
      	}
      }
      print_produced(item_to_produce);
      item_to_produce++;
      pthread_cond_signal(&cond_worker);
    }
      
    }
    pthread_cond_signal(&cond_worker);
  pthread_mutex_unlock(&lock);
  return 0;
}

void *worker_loop(void *data){
	int thread_id = *((int *)data);
//  printf("hey %d",thread_id);
    pthread_mutex_lock(&lock);
//    int thread_part=count++;
   /* int mul;
    if (max_buf_size%num_workers!=0){
    	mul=(max_buf_size/num_workers)+1;
    }
    else{
    	mul=max_buf_size/num_workers;
    }*/
    while(1){
//      printf("count %d\n",count);
      if(count >= total_items)
        break;

    	if (item_in_buf==0){
    	    pthread_cond_signal(&cond_master);
//          printf("hey %d",thread_id);
    	}

      else{
// 		pthread_cond_signal(&cond_worker);
    	for (int i=0;i<max_buf_size;i++){
    		/*if (i>=max_buf_size)
    			break;*/
    	
    			if (buf[i]!=-1){
    				print_consumed(buf[i],thread_id);
    				buf[i]=-1;
 //           printf("hey %d\n",thread_id);
    				item_in_buf=item_in_buf-1;
    			 break;
    		}
    	}
   }  
    if (item_in_buf>0){
      pthread_cond_signal(&cond_worker); 
    }
      pthread_cond_signal(&cond_master);
      if(count < total_items)
      pthread_cond_wait(&cond_worker,&lock);
 //     break;

    }
    pthread_cond_signal(&cond_worker);
    pthread_mutex_unlock(&lock);
    return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{
//  printf("hey %d",1);
  int master_thread_id = 0;
  pthread_t master_thread;
  item_to_produce = 0;
//  pthread_attr_t attr;

/*	pthread_mutex_init(&lock, NULL);
	pthread_cond_init (&cond_master, NULL);
	pthread_cond_init (&cond_worker, NULL);

	 For portability, explicitly create threads in a joinable state 
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);*/

  if (argc < 4) {
    printf("./master-worker #total_items #max_buf_size #num_workers e.g. ./exe 10000 1000 4\n");
    exit(1);
  }
  else {
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
//  printf("hey %d",1);
  // Initlization code for any data structures, variables, etc
  pthread_t threads[num_workers];
  buf=(int*)malloc(max_buf_size * sizeof(int));
//  count;
  item_in_buf=0;

  for (int i=0;i<max_buf_size;i++){
  	buf[i]=-1;
  }
//  printf("hey %d",1);
  //create master producer thread
  pthread_create(&master_thread, NULL, generate_requests_loop, (void *)&master_thread_id);
//  sleep(1);
  //create worker consumer threads
  int id[num_workers];
  for (int i=0;i<num_workers;i++){
    id[i]=i;
  }
  for (int i=0;i<num_workers;i++){
    pthread_create(&threads[i], NULL, worker_loop, (void *)&id[i]);     
  }

  //wait for all threads to complete
  for (int i=0;i<num_workers;i++){
  	pthread_join(threads[i],NULL);
  //  printf("heyaa%ld\n", threads[i]);
  }
  pthread_join(master_thread, NULL);
  printf("master joined\n");

  //deallocate and free up any memory you allocated
  free(buf);
//  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond_worker);
  pthread_cond_destroy(&cond_master);
  pthread_exit(NULL);
  return 0;
}
