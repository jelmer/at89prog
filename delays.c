/* 
	at89prog
	(c) 2003-2004 Jelmer Vernooij <jelmer@samba.org>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "delays.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

/** (based on some timing code in ponyprog) */

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
