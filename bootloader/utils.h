#ifndef _UTILS_H_
#define _UTILS_H_

#include "commands.h"
#include "memVals.h"
#include <stm32f30x_flash.h>
#include <core_cm4.h>
#include <core_cmFunc.h>

extern void jumpto(uint32_t);
extern void loadStackGo(uint32_t, uint32_t);

void btPinInit(void);
uint8_t bNOT8(uint8_t);
uint16_t bNOT16(uint16_t);
uint32_t bNOT32(uint32_t);
uint8_t readFlash8(uint32_t);
uint16_t readFlash16(uint32_t);
uint32_t readFlash32(uint32_t);
uint32_t revWord(uint32_t);
uint16_t revHalfWord(uint16_t);
void sleep(int);

void ack(void);
void nak(void);

uint8_t eraseRAM(void);
uint32_t recvAddr(uint8_t*);
uint8_t chkCmdComp(uint8_t);
uint8_t inRange(uint32_t, uint8_t);
uint32_t addrPageStart(uint32_t);
void disableAllIRQ();

void bootToUser(uint32_t);


#endif
