#include <Arduino.h>

void enableIMU(int sda = 21, int scl = 22);
void readACC(byte buff[]);
void readMAG(byte buff[]);
void readGYR(byte buff[]);


void writeTo(int device, byte address, byte val);
void readFrom(int device, byte address, int num, byte buff[]);

