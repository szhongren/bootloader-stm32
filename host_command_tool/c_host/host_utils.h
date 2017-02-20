#ifndef _HOST_UTILS_H_
#define _HOST_UTILS_H_

#define DEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

static uint32_t ser_baud_to_id(uint32_t);

void welcome();
void usage();
void help(char*);
void prompt();

int ser_open(const char[]);
void ser_setup(char[], int, uint32_t);

void debugSend(int, char*, int);
void debugRecv(int, int);
int sendByte(int, uint8_t);
int recvByte(int);

off_t fsize(const char*);

uint32_t revWord(uint32_t);
uint16_t revHalfWord(uint16_t);

#endif
