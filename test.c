#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <signal.h>
 

void set_timer();
void signal_handler();
int foo();

static int count = 0;
 
int main(int argc, char* argv[])
{
	signal(SIGALRM,signal_handler);
    
	set_timer();

	while(count < 10);
	exit(0);
    return 0;
}

void set_timer()
{
	struct itimerval itv;
 
 
	itv.it_value.tv_sec = 2;    //timer start after 3 seconds later
	itv.it_value.tv_usec = 0;
 
 
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
 
 
	setitimer(ITIMER_REAL,0,0);
    setitimer(ITIMER_REAL,&itv,0);
}
 
 
void signal_handler(int m)
{
	count ++;
	printf("%d\n",count);
}

 
