#ifndef _BTLDR_CMD_H_
#define _BTLDR_CMD_H_

#include "utils.h"
#include <f3d_uart.h>
#include <stm32f30x_flash.h>
#include <stm32f30x_misc.h>

#define VERS 0x01

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

void get(); // done
void getVers(); // done
void getID(); // done
void readMem(); // done
void go(); // kind of done
void writeMem();
void erase();
void writeP();
void writeU();
void readP(); // done
void readU(); // should be done, but rdpconfig is timing out for some reason
void test();

#endif
