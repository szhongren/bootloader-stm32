#ifndef _MEM_VALS_H_
#define _MEM_VALS_H_

#define CODE_BEGIN 0x00000000
#define MULTI_BEGIN 0x00000000
#define MULTI_END 0x0003FFFF
#define FLASH_BEGIN 0x08000000
#define FLASH_END 0x0803FFFF
#define CCM_BEGIN 0x10000000
#define CCM_END 0x10001FFF
#define SYSTEM_BEGIN 0x1FFFD800
#define SYSTEM_END 0x1FFFF7FF
#define OPT_BYTES_BEGIN 0x1FFFF800
#define OPT_BYTES_END 0x1FFFFFFF
#define CODE_END 0x1FFFFFFF
#define SRAM_BEGIN 0x20000000
#define SRAM_END 0x20009FFF

#define PAGE_SIZE 0x0800
#define APPLICATION_ADDRESS 0x08010000
#define APPLICATION_STACK 0x20002800

#endif
