/*----------xv6 sync lab----------*/
#include "types.h"
#include "x86.h"
#include "spinlock.h"
#include "defs.h"
#include "barrier.h"

//define any variables needed here
struct barrier{
	int cap;
	int counter;
	void* chan;
	struct spinlock lk;
} bar;


int
barrier_init(int n)
{
  //to be done
	bar.cap=n;
	bar.counter=0;
	bar.chan = (void*)1;
	initlock(&bar.lk, "r");
  return 0;
}



int
barrier_check(void)
{
  //to be done
	
	acquire(&bar.lk);
	bar.counter=bar.counter+1;
	if (bar.counter != bar.cap){
		
		sleep(bar.chan, &bar.lk);
	}
	else{
		wakeup(bar.chan);
	}
	release(&bar.lk);
  return 0;
}

/*----------xv6 sync lock end----------*/
