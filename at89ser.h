#ifndef __AT89SER_H__
#define __AT89SER_H__

int activate();
void deactivate();
void programming();

void writecode(int addr, char byte);
int readcode(int addr);

void writedata(int addr, char byte);
int readdata(int addr);

void erase();

void lock(int byte);

#endif
