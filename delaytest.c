#include "delays.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char **argv) 
{
	struct timeval tv1, tv2, tv3;
	gettimeofday(&tv1, NULL);
	waitmicrosec(1);
	gettimeofday(&tv2, NULL);
	timersub(&tv2, &tv1, &tv3);
	printf("New delays (microsec): %ld.%06ld\n", tv3.tv_sec, tv3.tv_usec);

	gettimeofday(&tv1, NULL);
	waitmillisec(1);
	gettimeofday(&tv2, NULL);
	timersub(&tv2, &tv1, &tv3);
	printf("New delays (millisec): %ld.%06ld\n", tv3.tv_sec, tv3.tv_usec);
	
	gettimeofday(&tv1, NULL);
	{ struct timespec ts = {0, 1000000ul * 1}; /* 0 sec, 1e6 ns = 1ms */ nanosleep(&ts, NULL); }
	gettimeofday(&tv2, NULL);
	timersub(&tv2, &tv1, &tv3);
	printf("Old delays (millisec): %ld.%06ld\n", tv3.tv_sec, tv3.tv_usec);


	gettimeofday(&tv1, NULL);
	{ struct timespec ts = {0, 1000ul * 1}; /* 0 sec, 1e9 ns = 1micros */ nanosleep(&ts, NULL);}
	gettimeofday(&tv2, NULL);
	timersub(&tv2, &tv1, &tv3);
	printf("Old delays (microsec): %ld.%06ld\n", tv3.tv_sec, tv3.tv_usec);

	return 0;
}
