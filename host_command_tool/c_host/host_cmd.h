#ifndef _HOST_CMD_H_
#define _HOST_CMD_H_

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "memVals.h"

enum CMD {
  CMD_INIT = 0x7F,
  CMD_GET = 0x00,
  CMD_GETVERS = 0x01,
  CMD_GETID = 0x02,
  CMD_READMEM = 0x11,
  CMD_GO = 0x21,
  CMD_WRITEMEM = 0x31,
  CMD_ERASE = 0x43,
  CMD_WRITE_PROTECT = 0x63,
  CMD_WRITE_UNPROTECT = 0x73,
  CMD_READ_PROTECT = 0x82,
  CMD_READ_UNPROTECT = 0x92,
  TEST = 0xAF
};

enum reply {
  ACK = 0x79,
  NAK = 0x1F
};

void SEND_CMD(int, uint8_t);
void INIT(int, int*);
void CMD_WRITE(int, uint32_t, uint32_t, uint8_t []);
void writeFile(int, char*, char*, int);
void CMD_READ(int, char*, char*, int);
void CMD_JUMP(int, char*, int);
void CMD_WRITEU(int);
void CMD_WRITEP(int);
void CMD_READU(int);
void CMD_READP(int);
void protect(int, char*, int);
void unprotect(int, char*, int);
void options(char*, char[], char*, unsigned int*);

uint32_t parseAddr(char*);
void sendAddr(int, uint32_t);

int expectACK(int, char*);


#endif
