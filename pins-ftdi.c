/* 
	at89prog - support for FTDI
	(c) 2003-2004 Jelmer Vernooij <jelmer@samba.org>
	(c) 2006 Stefan Lievens <zatalian@gmail.com>

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
#include <usb.h>
#include <ftdi.h>

static struct ftdi_context ftdic;
static int ftdi_data = 0;

static char *available_pins[] = { "P0", "P1", "P2", "P3", "P4", "P5", "P6", 
								  "P7", NULL };

static int ftdi_at98prog_init(char *location) 
{
	int fd, i;
	char bitmask = 0;
	
	ftdi_init(&ftdic);
	fd = ftdi_usb_open(&ftdic, 0x0403, 0x6001);
	if(fd < 0 && fd != -5) {
		perror("Error opening ftdi device");
		return -1;
	}

	i = ftdi_enable_bitbang(&ftdic, bitmask);
	i = ftdi_set_baudrate(&ftdic, 9600);
	
	ftdi_data = 0;
	
	return 0;
}

static int ftdi_at98prog_close()
{
	ftdi_disable_bitbang(&ftdic);
	ftdi_usb_close(&ftdic);
	ftdi_deinit(&ftdic);
	return 0;
}

static void ftdi_at98prog_set(int p)
{
	ftdi_data |= (0x01 << p);
	ftdi_write_data(&ftdic, (unsigned char *) &ftdi_data, 1);
}

static void ftdi_at98prog_clear(int p)
{

	ftdi_data &= ~(0x01 << p);
	ftdi_write_data(&ftdic, (unsigned char *) &ftdi_data, 1);
}

static int ftdi_at98prog_get(int p)
{
	int status;
	
	ftdi_read_pins(&ftdic, (unsigned char *) &status);
	if (status & (0x01 << p)) return 1; else return 0;
}

struct pins_backend ftdi = {
	"ftdi",
	available_pins,
	ftdi_at98prog_init,
	ftdi_at98prog_set,
	ftdi_at98prog_clear,
	ftdi_at98prog_get,
	ftdi_at98prog_close
};
