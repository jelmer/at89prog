#ifndef __PINS_H__
#define __PINS_H__

int readrcfile(char *name); 

struct pins_backend { 
	char *name;
	char **available_pins;
	int (*init)(char *loc);
	void (*set) (int);
	void (*clear) (int);
	int (*get) (int);
	int (*close) ();
	int (*config)(char *name, char *value);
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
