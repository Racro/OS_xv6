#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
	int ret;
	int i;

	ucounter_init();
	uspinlock_init();
	ret = fork();
	int var=10;
	int var2=11;
	if(ret == 0)
	{	
		
	  for(i=0; i < 10; i++) {
	  	uspinlock_acquire(1);
		ucounter_set(0,1);
	    printf(1, "I am child\n");
	    ucv_wakeup(var2);
	    ucv_sleep(var,1);

	    uspinlock_release(1);
	  }
	  uspinlock_acquire(1);
	  ucv_wakeup(var2);
      uspinlock_release(1);
          exit();
	}
	else
	{

	  for(i=0; i < 10; i++) {
	  	
	  	uspinlock_acquire(1);
	  	if (ucounter_get(0)!=1)
	  		ucv_sleep(var2,1);
	  	printf(1, "I am parent\n");
	  	ucv_wakeup(var);
	  	ucv_sleep(var2,1);
	  	uspinlock_release(1);
	  }
	  wait();
      exit();
	}
}

