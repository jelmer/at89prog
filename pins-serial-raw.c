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
	{ 3, 0x40, 1}, /* TXD */
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

