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
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/io.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "at89ser.h"
#include "pins.h"
#include "delays.h"

void SPI_Out(int b)
{
	int i;
	for(i = 7; i >= 0; i--) {
		if(b & (1 << i)) ClearMOSI();
		else SetMOSI();
		waitmicrosec(2);
		SetSCK();
		waitmicrosec(3);
		ClearSCK();
		waitmicrosec(2);
	}
}

int SPI_In()
{
	int i, b = 0;
	for(i = 7; i >= 0; i--) {
		SetSCK();
		waitmicrosec(2);
		if(GetMISO())b |= 1 << i;
		waitmicrosec(3);
		ClearSCK();
		waitmicrosec(2);
	}
	return b;
}

void programming()
{
	/* Send enable serial instruction to MOSI */
	SPI_Out(0xAC); /* 1010 1100 */
	SPI_Out(0x53); /* 0101 0011 */
	SPI_Out(0x00); /* xxxx xxxx (don't care) */
	waitmillisec(9);
}

void erase()
{
	SPI_Out(0xAC); /* 1010 1100 */
	SPI_Out(0x04); /* xxxx x100 (x = don't care) */
	SPI_Out(0x00); /* xxxx xxxx (don't care) */
	waitmillisec(16);
}


void writecode(int addr, char b)
{

	SPI_Out(0x02 | ((addr >> 5) & 0xF8) | ((addr >> 11) & 0x04)); /* hhhh h010 */
	SPI_Out(addr & 0xFF); /* llll llll */
	SPI_Out(b);
	waitmillisec(6);
}

int readcode(int addr)
{
	SPI_Out(0x01 | ((addr >> 5) & 0xF8) | ((addr >> 11) & 0x04)); /* hhhh h001 */
	SPI_Out(addr & 0xFF); /* llll llll */
	return SPI_In();
}

void writedata(int addr, char b)
{
	SPI_Out(0x06 | ((addr >> 5) & 0xF8));
	SPI_Out(addr & 0xFF); /* llll llll */
	SPI_Out(b);
}

int readdata(int addr)
{

	SPI_Out(0x05 | ((addr >> 5) & 0xF8));
	SPI_Out(addr & 0xFF); /* llll llll */
	return SPI_In();
}

int activate()
{
	deactivate();
	ClearSCK();
	SetMOSI();
	SetRST();
	waitmillisec(12);
	
	if(!GetCHK())return 0;
	return 1;
}

void deactivate()
{
	ClearRST();
	waitmillisec(12);
}

void lock(int byte)
{
	int mask = 0xff & ~byte;

	SPI_Out(0xAC);		/* 1010 1100 */
	SPI_Out(mask | 0x07);	/* pqrx x111 */
	SPI_Out(0);		/* xxxx xxxx */
	waitmillisec(9);
}
