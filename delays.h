#ifndef __DELAYS_H__
#define __DELAYS_H__

#define waitmillisec(n) { struct timespec ts = {0, 1000000ul * n}; /* 0 sec, 1e6 ns = 1ms */ nanosleep(&ts, NULL); }

#define waitmicrosec(n) { long long i = n * 10000; while(i)i--; }
	
#endif /* __DELAYS_H__ */
