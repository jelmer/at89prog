/* at89ser.c - Functions for programming a AT89S8252 over the 
 * serial port. 
 *
 * (C) 2003 Jelmer Vernooij <jelmer@samba.org>. Thanks to Hans Tjeerdsma 
 * for all the help with the debugging and research.
 */

#include "config.h"
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

int serport = 0x2f8;

#define INT_PORT   (serport+1)
#define CONF_PORT  (serport+3)
#define STATUS_OUT (serport+4)
#define STATUS_IN  (serport+6)

#define waitmillisec(n) { struct timespec ts = {0, 1000000ul * n}; /* 0 sec, 1e6 ns = 1ms */ nanosleep(&ts, NULL); }

#define waitmicrosec(n) { int i; for(i = 0; i < n; i++) outb(0, 0x80); }
	
inline void SetSCK() 
{ 
	int status = inb(STATUS_OUT);
	status |= 0x02; /* RTS, which is connected to SCK */
	outb(status, STATUS_OUT);
}

inline void ClearSCK() 
{ 
	int status = inb(STATUS_OUT);
	status &= ~0x02; /* RTS, which is connected to SCK */
	outb(status, STATUS_OUT);
}

inline void ClearMOSI() 
{
	int status = inb(CONF_PORT);
	status |= 0x40; /* TXD, which is connected to MOSI */
	outb(status, CONF_PORT);
}

inline void SetMOSI() 
{
	int status = inb(CONF_PORT);
	status &= ~0x40; /* TXD, which is connected to MOSI */
	outb(status, CONF_PORT);
}

/* 0x10 on STATUS_IN is CTS, aka MISO */
inline int GetMISO() 
{ 
	int r = !(inb(STATUS_IN) & 0x10);
	return r;
}

/* 0x20 on STATUS_IN is DSR, aka CHK */
inline int GetCHK() 
{ 
	return inb(STATUS_IN) & 0x20;
}

inline void SetRST() 
{ 
	int status = inb(STATUS_OUT);
	status |= 0x1; /* DTR, which is connected to RST */
	outb(status, STATUS_OUT);
}

inline void ClearRST() 
{ 
	int status = inb(STATUS_OUT);
	status &= ~ 0x1; /* DTR, which is connected to RST */
	outb(status, STATUS_OUT);
}

void SPI_Out(int b)
{
	int i;
	for(i = 7; i >= 0; i--) {
		if(b & (1 << i)) SetMOSI();
		else ClearMOSI();
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
	waitmillisec(9);
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
	ClearMOSI();
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
