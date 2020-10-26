#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
	rw -> lock = PTHREAD_MUTEX_INITIALIZER;
	rw -> read = PTHREAD_COND_INITIALIZER;
	rw -> write = PTHREAD_COND_INITIALIZER;

	rw->read_act = rw->write_act = rw->read_wait = rw->write_wait = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
	pthread_mutex_lock(&(rw->lock));
	while(rw->write_act || rw->write_wait>0){
		pthread_cond_wait(&(rw->read),&(rw->lock));
	}
	rw->read_wait++;
	pthread_mutex_unlock(&(rw->lock));
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
	pthread_mutex_lock(&(rw->lock));
	rw->read_wait--;
	if (rw->read_wait==0){
		pthread_cond_signal(&(rw->write));	
	}
	pthread_mutex_unlock(&(rw->lock));
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
	pthread_mutex_lock(&(rw->lock));
	rw->write_wait++;
	while(rw->write_act || rw->read_wait>0) {
		pthread_cond_wait(&(rw->write),&(rw->lock));
	}
	rw->write_act=1;
	rw->write_wait--;
	pthread_mutex_unlock(&(rw->lock));
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
	pthread_mutex_lock(&(rw->lock));
	rw->write_act=0;
	pthread_cond_signal(&(rw->write));
	pthread_cond_broadcast(&(rw->read));
	pthread_mutex_unlock(&(rw->lock));
}
