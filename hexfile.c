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
#include <sys/types.h>
#include <string.h>
#include <sys/io.h>
#include <stdlib.h>
#include <unistd.h>
#include "pins.h"
#include "at89ser.h"
#include "delays.h"
#include "hexfile.h"

int readhexline(FILE *fd, void **data, size_t *len, long *address)
{
	int checksum1, checksum2;
	int length, addr1, addr2, type;
	int j, byte;

	do { 
		checksum1 = 0;
		if(feof(fd)) {
			return HEX_FILE_END_OF_FILE;
		}

		if(fscanf(fd, ":%2x%2x%2x%2x", &length, &addr1, &addr2, &type) < 3) {
			return HEX_FILE_CORRUPT_LINE;
		}
	} while(type == 1 || type == 2);

	checksum1+=length;
	checksum1+=type;
	checksum1+=addr1;
	checksum1+=addr2;

	*address = addr1 * 0x100 + addr2;

	*data = malloc(length);
	*len = length;

	for(j = 0; j < length; j++) {
		if(fscanf(fd, "%2x", &byte) < 1) {
			return HEX_FILE_CORRUPT_LINE;
		}

		checksum1+=byte;
		((unsigned char *)(*data))[j] = byte;
	}

	if(fscanf(fd, "%2x", &checksum2) < 1) {
		return HEX_FILE_CORRUPT_LINE;
	}

	checksum1 &= 0xFF;
	checksum1 = (~checksum1 + 1) & 0xFF;

	if(checksum1 != checksum2) {
		return HEX_FILE_ERR_CHECKSUM;
	}

	/* Find the next newline so we're in position for the next read */
	while(!feof(fd)) { 
		byte = getc(fd); 
		if(byte != '\n' && byte != '\r') {
			ungetc(byte, fd);
			break; 
		}
	}
	return 0;
}
