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

#define PIN_MOSI 0
#define PIN_SCK  1
#define PIN_RST  2
#define PIN_CHK  3
#define PIN_MISO 4

char *location = NULL;
char *pin_names[] = { "MOSI", "SCK", "RST", "CHK", "MISO", NULL };
int pin_mapping[5] = { -1, -1, -1, -1, -1 };
int pin_reverse[5] = { 0, 0, 0, 0, 0 };

#define MAXLEN 100
#define MAXSETTINGS 100

static struct pins_backend *backend = NULL;
extern struct pins_backend serial_raw, serial, parallel;
static struct pins_backend *pins_backends[] = {
	&serial,
	&serial_raw,
	&parallel,
	NULL
};

static struct confsetting {
	char name[MAXLEN];
	char value[MAXLEN];
} confsettings[MAXSETTINGS];

static int num_confsettings = 0;

int get_pin_id(char *name)
{
	int i;
	for(i = 0; pin_names[i]; i++) {
		if(!strcmp(pin_names[i], name)) return i;
	}
	return -1;
}

int backend_get_pin_id(char *name)
{
	int i;
	for(i = 0; backend->available_pins[i]; i++) {
		if(!strcmp(backend->available_pins[i], name)) return i;
	}
	return -1;
}

int pins_read_config_file(char *name) 
{
	char bit[10], pin[10];
	int line = 0;
	FILE *fd = fopen(name, "r");

	if(!fd) { 
		fprintf(stderr, "Can't open %s!\n"
						"Type 'man 5 at89prog' for information about the RC file format\n", name);
		perror("fopen"); 
		return 1; 
	}

	while(!feof(fd)) 
	{
		line++;
		if(fscanf(fd, "%100s %100s\n", bit, pin) < 2) {
			fprintf(stderr, "Invalid line %d in %s, ignoring\n", line, name);
			continue;
		}

		if(!strcasecmp(bit, "type")) pins_set_backend(pin);
		else if(!strcasecmp(bit, "port")) pins_set_location(pin);
		else {
			if(num_confsettings > MAXSETTINGS) {
				fprintf(stderr, "Maximum number of %d configuration settings reached\n", num_confsettings);
				return -1;
			}
			strncpy(confsettings[num_confsettings].name, bit, MAXLEN);
			strncpy(confsettings[num_confsettings].value, pin, MAXLEN);
			num_confsettings++;
		}
	}

	fclose(fd);
	return 0;
}

int pins_set_backend(char *name) 
{
	int i;
	for(i = 0; pins_backends[i]; i++) {
		if(!strcmp(pins_backends[i]->name, name)) {
			backend = pins_backends[i];
			return 0;
		}
	}
	
	fprintf(stderr, "No such port type '%s'\n", name);
	return -1;
}

int pins_set_location(char *loc)
{
	location = strdup(loc);
	return 0;
}

int SetPinVariable(char *name, char *value) 
{
	int i, j;
	char reverse = 0;

	if(!name) {
		location = strdup(value);
		return 0;
	}

	if(name[0] == '!') {
		reverse = 1; 
		i = get_pin_id(name); 
	} else {
		i = get_pin_id(name);
	}

	if(i != -1) {
		j = backend_get_pin_id(value);
		if(j == -1) {
			return -2;
		}

		pin_mapping[i] = j;
		pin_reverse[i] = reverse;
		return 0;
	}

	return -1;
}

int pins_init()
{
	int i;
	if(!backend) backend = pins_backends[0];
	for(i = 0; i < num_confsettings; i++) {
		SetPinVariable(confsettings[i].name, confsettings[i].value);
	}

	return backend->init(location);
}



void SetMOSI() { 
	if(pin_mapping[PIN_MOSI] == -1) return;
	if(pin_reverse[PIN_MOSI]) backend->clear(pin_mapping[PIN_MOSI]);
	else backend->set(pin_mapping[PIN_MOSI]); 
}

void ClearMOSI() {
	if(pin_mapping[PIN_MOSI] == -1) return;
	if(pin_reverse[PIN_MOSI]) backend->set(pin_mapping[PIN_MOSI]);
	else backend->clear(pin_mapping[PIN_MOSI]); 
}

void SetSCK() { 
	if(pin_mapping[PIN_SCK] == -1) return;
	if(pin_reverse[PIN_SCK]) backend->clear(pin_mapping[PIN_SCK]);
	else backend->set(pin_mapping[PIN_SCK]); 
}

void ClearSCK() {
	if(pin_mapping[PIN_SCK] == -1) return;
	if(pin_reverse[PIN_SCK]) backend->set(pin_mapping[PIN_SCK]);
	else backend->clear(pin_mapping[PIN_SCK]); 
}

void SetRST() {
	if(pin_mapping[PIN_RST] == -1) return;
	if(pin_reverse[PIN_RST]) backend->clear(pin_mapping[PIN_RST]);
	else backend->set(pin_mapping[PIN_RST]); 
}

void ClearRST() {
	if(pin_mapping[PIN_RST] == -1) return;
	if(pin_reverse[PIN_RST]) backend->set(pin_mapping[PIN_RST]);
	else backend->clear(pin_mapping[PIN_RST]); 
}

int GetCHK() {
	if(pin_mapping[PIN_CHK] == -1) return -1;
	if(pin_reverse[PIN_CHK]) return !backend->get(pin_mapping[PIN_CHK]);
	return backend->get(pin_mapping[PIN_CHK]); 
}

int GetMISO() {
	if(pin_mapping[PIN_MISO] == -1) return -1;
	if(pin_reverse[PIN_MISO]) return !backend->get(pin_mapping[PIN_MISO]);
	return backend->get(pin_mapping[PIN_MISO]); 
}

int pins_fini() 
{ 
	if(backend->close) return backend->close(); 
	return 0;
} 
