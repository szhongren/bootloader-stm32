/* bootloader
 * does bootloader things like the integrated bootloader
 *
 */
#include "commands.h"
#include "utils.h"

#define APP_ADD 0x08010000

void bootloader() {
  while (1) {
    // wait for init byte
    uint8_t c = recvByte();
    if (c == CMD_INIT) {
      ack();
      break;
    }
  }

  while (1) {
    uint8_t cmd = recvByte();
    if (!chkCmdComp(cmd)) {
      nak();
    }
    switch(cmd) {
      case CMD_GET:
        get();
        break;
      case CMD_GETVERS:
        getVers();
        break;
      case CMD_GETID:
        getID();
        break;
      case CMD_READMEM:
        readMem();
        break;
      case CMD_GO:
        go();
        break;
      case CMD_WRITEMEM:
        writeMem();
        break;
      case CMD_ERASE:
        erase();
        break;
      case CMD_WRITE_PROTECT:
        writeP();
        break;
      case CMD_WRITE_UNPROTECT:
        writeU();
        break;
      case CMD_READ_PROTECT:
        readP();
        break;
      case CMD_READ_UNPROTECT:
        readU();
        break;
      case TEST:
        return;
      default:
        break;
    }
  }
}

int main(void) {

  f3d_uart_init();
  btPinInit();
  uint8_t bootPin = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0);
  sendByte(bootPin);
  // uint32_t CONTROL = __get_CONTROL();
  if (bootPin) {
    bootloader();
  } else {
    // goto user code
    // if (CONTROL) {
      sendByte(0xFF);  
    // }
    bootToUser(APP_ADD);
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
/* Infinite loop */
/* Use GDB to find out why we're here */
  while (1);
}
#endif

/* main.c ends here */
