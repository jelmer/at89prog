#include "delays.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

void waitmicrosec(int n)
{
	struct timeval t1, t2;

	gettimeofday(&t1, NULL);
	t2.tv_sec = 0; t2.tv_usec = n;
	timeradd(&t1, &t2, &t1);
	do {
		gettimeofday(&t2, NULL);
	} while (timercmp(&t2, &t1, <));
}

void waitmillisec(int n)
{
	waitmicrosec(n*1000);
}
