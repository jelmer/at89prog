#ifndef __PINS_H__
#define __PINS_H__

extern int serport;

void SetSCK();
void ClearSCK();
void SetMOSI();
void ClearMOSI();
int GetMISO();
void SetRST();
void ClearRST();
int GetCHK();

int readrcfile(char *name); 

#endif
