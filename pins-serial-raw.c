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

#include <stdio.h>
#include "pins.h"
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/io.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

static char *available_pins[] = { "RTS", "TXD", "CTS", "DSR", "DTR", NULL };

struct bit {
	int offset;
	int bit;
	char reverse;
};

static int serport = 0x3f8;

static struct bit bits[] = {
	{ 4, 0x02, 0}, /* RTS */
	{ 3, 0x40, 0}, /* TXD */
	{ 6, 0x10, 1}, /* CTS */
	{ 6, 0x20, 0}, /* DSR */
	{ 4, 0x01, 0},  /* DTR */
	{ 0, 0, 0 }
};

static void ser_set(int p)
{
	int status = inb(serport + bits[p].offset);
	
	if(bits[p].reverse)status &= ~bits[p].bit;
	else status |= bits[p].bit;
	outb(status, serport + bits[p].offset);
}

static void ser_clear(int p)
{
	int status = inb(serport + bits[p].offset);
	if(bits[p].reverse)status |= bits[p].bit;
	else status &= ~bits[p].bit;
	outb(status, serport + bits[p].offset);
}

static int ser_get(int p)
{
	int status = inb(serport + bits[p].offset);
	if(bits[p].reverse) return !(status & bits[p].bit);
	else return status & bits[p].bit;
}

static int raw_init(char *location) 
{
	if(location) serport = strtol(location, NULL, 16);

	if(ioperm(serport, 7, 1) == -1) 
	{
		perror("ioperm");
		fprintf(stderr, "Run at89prog with IO port access\n");
		return -1;
	}

	return 0;
}

struct pins_backend serial_raw = {
	"serial-raw",
	available_pins,
	raw_init,
	ser_set,
	ser_clear,
	ser_get,
	NULL
};

