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

static struct pins_backend *backend = NULL;
extern struct pins_backend serial_raw, serial;
static struct pins_backend *pins_backends[] = {
	&serial_raw,
	&serial,
	NULL
};

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

int LoadPinBackend(char *name) 
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

int BackendInit()
{
	return backend->init(location);
}

int ClosePinBackend() 
{ 
	if(backend->close) return backend->close(); 
	return 0;
} 
