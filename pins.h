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
#ifndef __PINS_H__
#define __PINS_H__

struct pins_backend { 
	char *name;
	char **available_pins;
	int (*init)(char *loc);
	void (*set) (int);
	void (*clear) (int);
	int (*get) (int);
	int (*close) ();
};

int BackendInit();

void SetMOSI();
void ClearMOSI();
void SetRST();
void ClearRST();
void SetSCK();
void ClearSCK();
int ClosePinBackend();
int GetCHK();
int GetMISO();
int SetPinVariable(char *name, char *Value);
int LoadPinBackend(char *name);

#endif
