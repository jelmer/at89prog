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
#include <popt.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/io.h>
#include <stdlib.h>
#include <unistd.h>
#include "pins.h"
#include "at89ser.h"
#include "delays.h"
#include "hexfile.h"

#define VERSION		"0.7"

char progress = 0, ignore_errors = 0, dump = 0;

char *port = NULL;

void usage(poptContext pc)
{
	poptPrintHelp(pc, stderr, 0);
	fprintf(stderr, "\nAvailable commands: \n"
			"\terase\n"
			"\treset\n"
			"\tlock <level>\n"
			"\twritefile [file] ...\n"
			"\treadfile <len> [file] ...\n"
			"\twritebyte <address> <byte>\n"
			"\treadbyte <address>\n"
			"\tversion\n");
}

void quit(int s)
{
	pins_fini();
	fprintf(stderr, "Received signal, exiting...\n");
	exit(0);
}


void writechar(char datamem, char do_verify, int address, int byte)
{
	int d;
	if(datamem)writedata(address, byte);
	else writecode(address, byte);

	if(dump) { printf("%02x ", byte); fflush(stdout); }

	if(do_verify) {
		if(datamem)d = readdata(address);
		else d = readcode(address);

		if(byte != d) {
			fprintf(stderr, "Error verifying byte at offset 0x%x\n", address);
			if(!ignore_errors) exit(1);
		}
	}
	if(progress)fputc('.', stderr);
}

int writebin(FILE *fd, char do_verify, char datamem)
{
	int i = 0;
	while(!feof(fd)) 
	{
		writechar(datamem, do_verify, i, fgetc(fd));
		i++;
	}
	if(progress)fputc('\n', stderr);
	return 0;
}

int writehex(FILE *fd, char do_verify, char datamem)
{
	int length; 
	void *data;
	long address;
	int i = 0, j = 0;
	int errors = 0;
	while(!feof(fd)) 
	{
		i++;
		switch(readhexline(fd, &data, &length, &address)) {
		case HEX_FILE_ERR_CHECKSUM:
			fprintf(stderr, "Checksum error in intel hex file, line %d\n", i);
			errors++;
			break;
		case HEX_FILE_CORRUPT_LINE:
			fprintf(stderr, "Corrupt line %d in intel hex file\n", i);
			errors++;
			return errors;
		case HEX_FILE_UNKNOWN_TYPE:
			fprintf(stderr, "Unknown file type at line %d in intel hex file\n", i);
			errors++;
			break;
		case HEX_FILE_END_OF_FILE:
			if(progress)fputc('\n', stderr);
			return errors;
		default:
			for(j = 0; j < length; j++) {
				writechar(datamem, do_verify, address+j, ((unsigned char *)data)			[j]);
			}
			break;
		}
	}
	if(progress)fputc('\n', stderr);
	return errors;
}

int main(int argc, const char **argv) 
{
	int datamem = 0, codemem = 0, verbose = 0, do_verify = 0, ignore_chk = 0;
	FILE *fd;
	char *format = "auto";
	char *rcfile = NULL;
	char c, print_usage = 1;
	char *backendname = NULL, *location = NULL;
	struct poptOption long_options[] = {
		POPT_AUTOHELP
		{ "data-memory", 'd', POPT_ARG_NONE, &datamem, 0, "Write specified file to data memory" },
		{ "code-memory", 'c', POPT_ARG_NONE, &codemem, 0, "Write specified file to code memory (default)" },
		{ "dump", 'D', POPT_ARG_NONE, &dump, 0, "Dump written bytes to screen" },
		{ "format", 'f', POPT_ARG_STRING, &format, 0, "File format (auto,hex,bin)" },
		{ "ignore-chk", 'i', POPT_ARG_NONE, &ignore_chk, 0, "Don't wait for CHK to confirm RST" },
		{ "ignore-errors", 'I', POPT_ARG_NONE, &ignore_errors, 0, "Ignore errors" },
		{ "progress", 'P', POPT_ARG_NONE, &progress, 0, "Print progress dots" },
		{ "verify", 'V', POPT_ARG_NONE, &do_verify, 0, "Verify written bytes" }, 
		{ "type", 't', POPT_ARG_STRING, &backendname, 't', "Type of port to use (serial, parallel or serial-raw)" },
		{ "port", 'p', POPT_ARG_STRING, &location, 'p', "Address of port to use" },
		{ "rcfile", 'r', POPT_ARG_STRING, &rcfile, 0, "Use rc file from specified location" },
		{ "verbose", 'v', POPT_ARG_NONE, &verbose, 0, "Be verbose" },
		POPT_TABLEEND
	};

	poptContext pc;

	pc = poptGetContext(NULL, argc, argv, long_options, 0);
	poptSetOtherOptionHelp(pc, "command [file-to-write]");

	while ((c = poptGetNextOpt(pc)) != -1) { }

	if(!poptPeekArg(pc)) 
	{
		usage(pc);
		return 0;
	}

	if(!rcfile) { 
		rcfile = malloc(strlen(getenv("HOME")) + 20);
		snprintf(rcfile, strlen(getenv("HOME")) + 20, "%s/.at89progrc", getenv("HOME")); 
	}

	if(rcfile) {
		if(pins_read_config_file(rcfile) != 0) return 1;
	}
	
	if(backendname && pins_set_backend(backendname) != 0) return 1;
	if(location && pins_set_location(location) != 0) return 1;
	if(pins_init() != 0) return 1;

	signal(SIGINT, quit);
	signal(SIGSEGV, quit);

	if(!activate() && !ignore_chk)
	{
		fprintf(stderr, "RST set, but CHK is low\n");
		return 1;
	}

	atexit(deactivate);
	
	if(!strcmp(poptPeekArg(pc), "reset"))
	{ 
		print_usage = 0;
		deactivate(); 
		if(verbose) fprintf(stderr, "Microcontroller has been reset.\n");
	} else if(!strcmp(poptPeekArg(pc), "erase"))
	{
		print_usage = 0;
		programming();
		erase();
		if(verbose) fprintf(stderr, "Microcontroller memory has been erased.\n");
	} else if(!strcmp(poptPeekArg(pc), "lock"))
	{
		int lock_level;
		poptGetArg(pc);
		lock_level = atoi(poptGetArg(pc));
		print_usage = 0;
		programming();
		lock(lock_level);
		
		if(verbose) fprintf(stderr, "Locked at level %d\n", lock_level);
	} else if(!strcmp(poptPeekArg(pc), "writefile"))
	{
		int errors;
		poptGetArg(pc);
		programming();
		while(poptPeekArg(pc)) {
			int firstchar;
			const char *filename = poptGetArg(pc);
			
			if(!strcmp(filename, "-")) fd = stdin;
			else { 
				fd = fopen(filename, "r");
				if(!fd) {
					fprintf(stderr, "Unable to open file %s, ignoring.\n", filename);
					continue;
				}
			}

			firstchar = getc(fd);
			ungetc(firstchar, fd);

			if(!strcmp(format, "hex") || (!strcmp(format, "auto") && firstchar == ':'))errors = writehex(fd, do_verify, datamem);
			else if(!strcmp(format, "auto") || !strcmp(format, "bin"))errors = writebin(fd, do_verify, datamem);
			else {
				fprintf(stderr, "Unknown format %s, ignoring file\n", format);
				fclose(fd);
				continue;
			}

			if(!errors && verbose)fprintf(stderr, "File %s programmed correctly\n", filename);
			else if(verbose)fprintf(stderr, "File %s programmed with %d errors\n", filename, errors);
			fclose(fd);
		}
	} else if(!strcmp(poptPeekArg(pc), "readfile")) {
		int len, i;
		programming();
		poptGetArg(pc);

		if(!poptPeekArg(pc)) {
			fprintf(stderr, "readfile needs at least one argument (number of bytes to read\n");
			return 1;
		}

		len = atol(poptGetArg(pc));
		
		if(!poptPeekArg(pc)) fd = stdout;
		else {
			fd = fopen(poptGetArg(pc), "w+");
			if(!fd) {
				perror("fopen");
				return 1;
			}
		}

		if(!strcmp(format, "bin"))fprintf(stderr, "Warning: writing in binary mode\n");

		for(i = 0; i < len; i++) {
			if(datamem)fputc(readdata(i), fd);
			else fputc(readcode(i), fd);
			if(progress)fputc('.', stderr);
		}

		if(progress)fputc('\n', stderr);
			
		if(verbose)fprintf(stderr, "%d bytes read\n", len);
		fclose(fd);
	} else if(!strcmp(poptPeekArg(pc), "writebyte")) {
		int address, byte;
		poptGetArg(pc);
		programming();
		if(!poptPeekArg(pc)) {
			fprintf(stderr, "writebyte requires 2 arguments\n");
			return 1;
		}
		address = strtol(poptGetArg(pc), NULL, 0);

		if(!poptPeekArg(pc)) {
			fprintf(stderr, "writebyte requires 2 arguments\n");
			return 1;
		}
		byte = strtol(poptGetArg(pc), NULL, 0);

		writechar(datamem, do_verify, address, byte);
		if(verbose)fprintf(stderr, "%x written to %x\n", byte, address);
	} else if(!strcmp(poptPeekArg(pc), "readbyte")) {
		int address;
		poptGetArg(pc);
		programming();
		if(!poptPeekArg(pc)) {
			fprintf(stderr, "readbyte requires an argument\n");
			return 1;
		}
		address = strtol(poptGetArg(pc), NULL, 0);

		if(datamem) printf("%x\n", readdata(address));
		else printf("%x\n", readcode(address));
	} else if(!strcmp(poptPeekArg(pc), "version")) {
		  fprintf(stderr, "at89prog - a AT89S8252 programmer using SPI\n");
		  fprintf(stderr, " Version "VERSION"\n");
		  fprintf(stderr, " (C) 2003 Jelmer Vernooij <jelmer@samba.org>\n");
		  fprintf(stderr, "  Published under the GNU GPL\n");
		  return 0;
	} else {
		fprintf(stderr, "Unknown command %s\n", poptGetArg(pc));
		usage(pc);
	}

	poptFreeContext(pc);
	
	return 0;
}
