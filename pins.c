#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/io.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

struct pin {
	char *name;
	int offset;
	int bit;
	char reverse;
};

int serport = 0x3f8;

struct pin pins[] = {
	{ "RTS", 4, 0x02, 0},
	{ "TXD", 3, 0x40, 1},
	{ "CTS", 6, 0x10, 1},
	{ "DSR", 6, 0x20, 0},
	{ "DTR", 4, 0x1, 0},
	{ NULL }
};

int MOSI_Pin = 1, 
	MISO_Pin = 2, 
	RST_Pin = 4, 
	CHK_Pin = 3, 
	SCK_Pin = 0;

char MOSI_Reverse = 0,
	MISO_Reverse = 0, 
	RST_Reverse = 0, 
	CHK_Reverse = 0, 
	SCK_Reverse = 0;

inline void Set(struct pin pin, char reverse)
{
	int status = inb(serport + pin.offset);
	if(pin.reverse != reverse)reverse = 1;
	if(reverse)status &= ~pin.bit;
	else status |= pin.bit;
	outb(status, serport + pin.offset);
}

inline void Clear(struct pin pin, char reverse)
{
	int status = inb(serport + pin.offset);
	if(pin.reverse != reverse)reverse = 1;
	if(reverse)status |= pin.bit;
	else status &= ~pin.bit;
	outb(status, serport + pin.offset);
}

inline int Get(struct pin pin, char reverse)
{
	int status = inb(serport + pin.offset);
	if(pin.reverse != reverse)reverse = 1;
	if(reverse) return !(status & pin.bit);
	else return status & pin.bit;
}


void SetMOSI() { Set(pins[MOSI_Pin], MOSI_Reverse);	}
void ClearMOSI() { Clear(pins[MOSI_Pin], MOSI_Reverse); }
void SetSCK() { Set(pins[SCK_Pin], SCK_Reverse); }
void ClearSCK() { Clear(pins[SCK_Pin], SCK_Reverse); }
void SetRST() { Set(pins[RST_Pin], RST_Reverse); }
void ClearRST() { Clear(pins[RST_Pin], RST_Reverse); }
int GetCHK() { return Get(pins[CHK_Pin], CHK_Reverse); }
int GetMISO() { return Get(pins[MISO_Pin], MISO_Reverse); }

int findpin(char *name)
{
	int i;
	for(i = 0; pins[i].name; i++)
	{
		if(!strcmp(pins[i].name, name))return i;
	}

	return -1;
}

int readrcfile(char *name)
{
	char bit[10], pin[10], pinid = 0;
	char reverse;
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

		if(!strcmp(bit, "base")) 
		{
			serport = strtol(pin, NULL, 16);
			continue;
		}

		if(pin[0] == '!') reverse = 1;
		else reverse = 0;
		
		pinid = findpin(pin+reverse);

		if(pinid == -1) 
		{ 
			fprintf(stderr, "Invalid pin name %s in line %d, ignoring\n", pin + reverse, line); 
			continue; 
		}

		if(!strcmp(bit, "MOSI")) { MOSI_Pin = pinid; MOSI_Reverse = reverse; }
		else if(!strcmp(bit, "MISO")) { MISO_Pin = pinid; MISO_Reverse = reverse; }
		else if(!strcmp(bit, "RST")) { RST_Pin = pinid; RST_Reverse = reverse; }
		else if(!strcmp(bit, "SCK")) { SCK_Pin = pinid; SCK_Reverse = reverse; }
		else if(!strcmp(bit, "CHK")) { CHK_Pin = pinid; CHK_Reverse = reverse; }
		else { 
			fprintf(stderr, "Invalid bit name %s in line %d, ignoring\n", bit, line); 
			continue; 
		}
		
	}
	
	fclose(fd);
	return 0;
}

