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
#include <sys/termios.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/io.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

static int fd = 0;

#define SER_PIN_TXD 0
#define SER_PIN_DTR 1
#define SER_PIN_DSR 2
#define SER_PIN_RTS 3
#define SER_PIN_CTS 4 

static char *available_pins[] = { "TXD", "DTR", "DSR", "RTS", "CTS", NULL };

static int ser_init(char *location) 
{
	fd = open(location?location:"/dev/ttyS0", O_RDWR | O_NOCTTY);
	if(fd < 0) {
		perror("Error opening serial port");
		return -1;
	}

	return 0;
}

static int ser_close()
{
	close(fd);
	return 0;
}

static void ser_set(int p)
{
	int status;

	if(ioctl (fd, TIOCMGET, &status) < 0) {
		perror("ioctl");
		return;
	}
	
	switch(p) {
	case SER_PIN_TXD: 
		if(ioctl (fd, TIOCCBRK, 0) < 0) perror("ioctl");
		break;
	case SER_PIN_DTR: status |= TIOCM_DTR; break;
	case SER_PIN_DSR: status |= TIOCM_LE; break;
	case SER_PIN_RTS: status |= TIOCM_RTS; break;
	case SER_PIN_CTS: status |= TIOCM_CTS; break;
	}
	if(ioctl (fd, TIOCMSET, &status) < 0) perror("ioctl");
}

static void ser_clear(int p)
{
	int status;

	if(ioctl (fd, TIOCMGET, &status) < 0) {
		perror("ioctl");
		return;
	}
	
	switch(p) {
	case SER_PIN_TXD: 
		if(ioctl (fd, TIOCSBRK, 0) < 0) perror("ioctl"); 
		  break;
	case SER_PIN_DTR: status &=~TIOCM_DTR; break;
	case SER_PIN_DSR: status &=~TIOCM_LE; break;
	case SER_PIN_RTS: status &=~TIOCM_RTS; break;
	case SER_PIN_CTS: status &=~TIOCM_CTS; break;
	default:
		fprintf(stderr, "Write not available for pin %s\n", available_pins[p]);
		break;
	}
	
	if(ioctl (fd, TIOCMSET, &status) < 0) perror("ioctl");
}

static int ser_get(int p)
{
	int status;
	if(ioctl(fd, TIOCMGET, &status) < 0) perror("ioctl");
	switch(p) {
	case SER_PIN_DTR: return (status & TIOCM_DTR?1:0); 
	case SER_PIN_RTS: return (status & TIOCM_RTS?1:0);
	case SER_PIN_DSR: return (status & TIOCM_DSR?1:0);
	case SER_PIN_CTS: return (status & TIOCM_CTS?0:1);
	default:
		fprintf(stderr, "Read not available for pin %s\n", available_pins[p]);
		return -1;
	}
}

struct pins_backend serial = {
	"serial",
	available_pins,
	ser_init,
	ser_set,
	ser_clear,
	ser_get,
	ser_close
};
