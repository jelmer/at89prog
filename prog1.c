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

#define VERSION		"0.6"

char progress = 0;

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
	deactivate();
	ClosePinBackend();
	fprintf(stderr, "Received signal, exiting...\n");
	exit(0);
}

void writechar(char datamem, char do_verify, int address, int byte)
{
	int d;
	if(datamem)writedata(address, byte);
	else writecode(address, byte);

	if(do_verify) {
		if(datamem)d = readdata(address);
		else d = readcode(address);

		if(byte != d) {
			fprintf(stderr, "Error verifying byte at offset 0x%x\n", address);
			deactivate();
			exit(1);
		}
	}
}

int writebin(FILE *fd, char do_verify, char datamem)
{
	int i = 0;
	while(!feof(fd)) 
	{
		writechar(datamem, do_verify, i, fgetc(fd));
		i++;
		if(progress)fputc('.', stderr);
	}
	if(progress)fputc('\n', stderr);
	return 0;
}

int writehex(FILE *fd, char do_verify, char datamem)
{
	int length; int addr1, addr2; int type;
	long address;
	int errors = 0;
	int checksum1, checksum2;
	int i, j, byte;
	i = 0;
	while(!feof(fd)) 
	{
		checksum1 = 0;
		i++;
		if(fscanf(fd, ":%2x%2x%2x%2x", &length, &addr1, &addr2, &type) < 3) {
			fprintf(stderr, "Error reading intel hex file, line %d\n", i);
			deactivate();
			exit(1);
		}

		checksum1+=length;
		checksum1+=type;
		checksum1+=addr1;
		checksum1+=addr2;

		address = addr1 * 0x100 + addr2;

		if(type == 1) break;

		if(type == 2) continue;
		
		for(j = 0; j < length; j++) {
			if(fscanf(fd, "%2x", &byte) < 1) {
				fprintf(stderr, "Error reading byte %d in intel hex file, line %d\n", j, i);
				deactivate();
				exit(1);
			}

			checksum1+=byte;
			writechar(datamem,do_verify,address+j,byte);
		}

		if(fscanf(fd, "%2x", &checksum2) < 1) {
			fprintf(stderr, "Error reading checksum in intel hex file, line %d\n", i);
			deactivate();
			exit(1);
		}

		if((0x100 - (checksum1 & 0xFF)) != checksum2) {
			fprintf(stderr, "Warning: checksums do NOT match in intel hex file, line %d\n"
			"(%x != %x)\n" 
					"File may be corrupt\n", i, 0x100 - (checksum1 & 0xFF), checksum2);
			errors++;
		}

		while(!feof(fd)) { 
			byte = getc(fd); 
			if(byte != '\n' && byte != '\r') {
				ungetc(byte, fd);
				break; 
			}
		}

		if(progress)fputc('.', stderr);
	}
	if(progress)fputc('\n', stderr);
	return errors;
}


int read_pin_configuration(char *name)
{
	char bit[10], pin[10];
	int line = 0;
	FILE *fd = fopen(name, "r");

	if(!fd) { 
		fprintf(stderr, "Can't open %s!\n", name);
		perror("fopen"); 
		return 1; 
	}

	while(!feof(fd)) 
	{
		line++;
		if(fscanf(fd, "%4s %6s\n", bit, pin) < 2) {
			fprintf(stderr, "Invalid line %d in %s, ignoring\n", line, name);
			continue;
		}

		switch(SetPinVariable(bit, pin)) {
		case -1: 
			fprintf(stderr, "Invalid configuration variable '%s' at line %d in %s, ignoring\n", bit, line, name);
			continue;

		case -2:
			fprintf(stderr, "Invalid value '%s' for configuration variable '%s' at line %d in %s, ignoring\n", pin, bit, line, name);
			continue;

		default:
			break;
		}
	}
	
	fclose(fd);
	return 0;
}

int main(int argc, const char **argv) 
{
	int datamem = 0, codemem = 0, verbose = 0, do_verify = 0, ignore_chk = 0;
	FILE *fd;
	char *format = "auto";
	char *rcfile = NULL;
	char *type = "serial";
	char *port = NULL;
	char c, print_usage = 1;
	struct poptOption long_options[] = {
		POPT_AUTOHELP
		{ "data-memory", 'd', POPT_ARG_NONE, &datamem, 0, "Write specified file to data memory" },
		{ "code-memory", 'c', POPT_ARG_NONE, &codemem, 0, "Write specified file to code memory (default)" },
		{ "format", 'f', POPT_ARG_STRING, &format, 0, "File format (auto,hex,bin)" },
		{ "ignore-chk", 'i', POPT_ARG_NONE, &ignore_chk, 0, "Don't wait for CHK to confirm RST" },
		{ "progress", 'P', POPT_ARG_NONE, &progress, 0, "Print progress dots" },
		{ "verify", 'V', POPT_ARG_NONE, &do_verify, 0, "Verify written bytes" }, 
		{ "type", 't', POPT_ARG_STRING, &type, 't', "Type of port to use (serial or serial-raw)" },
		{ "port", 'p', POPT_ARG_STRING, &port, 'p', "Location of port to use" },
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
	
	if(LoadPinBackend(type) != 0) return 1;
	if(port)SetPinVariable(NULL, port);
	if(BackendInit() != 0) return 1;
	
	if(rcfile) {
		if(read_pin_configuration(rcfile) != 0) return 1;
	}

	signal(SIGINT, quit);
	signal(SIGSEGV, quit);

	if(!activate() && !ignore_chk)
	{
		fprintf(stderr, "RST set, but CHK is low\n");
		return 1;
	}
	
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
			deactivate();
			return 1;
		}

		len = atol(poptGetArg(pc));
		
		if(!poptPeekArg(pc)) fd = stdout;
		else {
			fd = fopen(poptGetArg(pc), "w+");
			if(!fd) {
				perror("fopen");
				deactivate();
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
			deactivate();
			return 1;
		}
		address = strtol(poptGetArg(pc), NULL, 16);

		if(!poptPeekArg(pc)) {
			fprintf(stderr, "writebyte requires 2 arguments\n");
			deactivate();
			return 1;
		}
		byte = strtol(poptGetArg(pc), NULL, 16);

		writechar(datamem, do_verify, address, byte);
		if(verbose)fprintf(stderr, "%x written to %x\n", byte, address);
	} else if(!strcmp(poptPeekArg(pc), "readbyte")) {
		int address;
		poptGetArg(pc);
		programming();
		if(!poptPeekArg(pc)) {
			fprintf(stderr, "writebyte requires 2 arguments\n");
			deactivate();
			return 1;
		}
		address = strtol(poptGetArg(pc), NULL, 16);

		if(datamem) printf("%x\n", readdata(address));
		else printf("%x\n", readcode(address));
	} else if(!strcmp(poptPeekArg(pc), "version")) {
		  fprintf(stderr, "at89prog - a AT89S8252 programmer over the serial port\n");
		  fprintf(stderr, " Version "VERSION"\n");
		  fprintf(stderr, " (C) 2003 Jelmer Vernooij <jelmer@samba.org>\n");
		  fprintf(stderr, "  Published under the GNU GPL\n");
		  return 0;
	} else {
		fprintf(stderr, "Unknown command %s\n", poptGetArg(pc));
		usage(pc);
	}

	deactivate();
	poptFreeContext(pc);
	
	return 0;
}
