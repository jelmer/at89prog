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

static char *available_pins[] = { "ERROR", "SLCT", "PE", "ACK", "BUSY", "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "STROBE", "AUTO_FD_XT", "INIT", "SLCT_IN", NULL };

struct bit {
	int offset;
	int bit;
	char reverse;
};

static int parport = 0x3bc;

static struct bit bits[] = {
	{ 1, 0x08, 0}, /* ERROR */
	{ 1, 0x10, 0}, /* SLCT */
	{ 1, 0x20, 0}, /* PE */
	{ 1, 0x40, 0}, /* ACK */
	{ 1, 0x80, 1}, /* BUSY */
	{ 0, 0x01, 0}, /* D0 */
	{ 0, 0x02, 0}, /* D1 */
	{ 0, 0x04, 0}, /* D2 */
	{ 0, 0x08, 0}, /* D3 */
	{ 0, 0x10, 0}, /* D4 */
	{ 0, 0x20, 0}, /* D5 */
	{ 0, 0x40, 0}, /* D6 */
	{ 0, 0x80, 0}, /* D7 */
	{ 2, 0x01, 1}, /* STROBE */
	{ 2, 0x02, 1}, /* AUTO_FD_XT */
	{ 2, 0x04, 0}, /* INIT */
	{ 2, 0x08, 1}, /* SLCT_IN */
	{ 0, 0, 0 }
};

static void par_set(int p)
{
	int status = inb(parport + bits[p].offset);
	
	if(bits[p].reverse)status &= ~bits[p].bit;
	else status |= bits[p].bit;
	outb(status, parport + bits[p].offset);
}

static void par_clear(int p)
{
	int status = inb(parport + bits[p].offset);
	if(bits[p].reverse)status |= bits[p].bit;
	else status &= ~bits[p].bit;
	outb(status, parport + bits[p].offset);
}

static int par_get(int p)
{
	int status = inb(parport + bits[p].offset);
	if(bits[p].reverse) return !(status & bits[p].bit);
	else return status & bits[p].bit;
}

static int raw_init(char *location) 
{

	if(location) parport = strtol(location, NULL, 16);

	if(ioperm(parport, 7, 1) == -1) 
	{
		perror("ioperm");
		fprintf(stderr, "Run at89prog with IO port access\n");
		return -1;
	}

	return 0;
}

struct pins_backend parallel = {
	"parallel",
	available_pins,
	raw_init,
	par_set,
	par_clear,
	par_get,
	NULL
};

