#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

using namespace std;

struct read_write_lock
{
	pthread_mutex_t lock; //lock
	pthread_cond_t read; //wait 
	pthread_cond_t write;

	//which one is active
	int read_act;
	int write_act;

	//which one is waiting
	int read_wait;
	int write_wait;
};

void InitalizeReadWriteLock(struct read_write_lock * rw);
void ReaderLock(struct read_write_lock * rw);
void ReaderUnlock(struct read_write_lock * rw);
void WriterLock(struct read_write_lock * rw);
void WriterUnlock(struct read_write_lock * rw);
