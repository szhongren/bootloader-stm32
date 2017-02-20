#include "commands.h"

void get() {
  ack();
  sendByte(0x0b);
  sendByte(VERS);
  sendByte(CMD_GET);
  sendByte(CMD_GETVERS);
  sendByte(CMD_GETID);
  sendByte(CMD_READMEM);
  sendByte(CMD_GO);
  sendByte(CMD_WRITEMEM);
  sendByte(CMD_ERASE);
  sendByte(CMD_WRITE_PROTECT);
  sendByte(CMD_WRITE_UNPROTECT);
  sendByte(CMD_READ_PROTECT);
  sendByte(CMD_READ_UNPROTECT);
  ack();
}

void getVers() {
  ack();
  sendByte(VERS);
  sendByte(0x00); // filler
  sendByte(0x00); // filler
  ack();
}

void getID() {
  ack();
  sendByte(0x01);
  sendByte(0x04);
  sendByte(0x22);
  ack();
}

void readMem() {
  if (0 /*FLASH_OB_GetRDP() != SET*/) {
    // chk if cmd is followed by complement and that RDP not set
    nak();
    return;
  }

  ack(); // ack that command is complemented and RDP not active

  uint32_t addr;
  uint8_t chksm = 0;
  addr = recvAddr(&chksm);
  uint8_t host_chksm = recvByte();
  if (!inRange(addr, CMD_READMEM) || host_chksm != chksm) {
    nak();
    return;
  }

  ack(); // ack address and chksm ok

  uint8_t count = recvByte();
  host_chksm = recvByte();
  if (count != bNOT8(host_chksm)) {
    nak();
    return;
  }

  ack(); // ack count and checksum

  uint32_t i;
  for (i = 0; i < count + 1; ++i) {
    sendByte(readFlash8(addr + i));
  }
}

void catchFunction();

void go() {
  if (0/*FLASH_OB_GetRDP() != SET*/) {
    // chk if cmd is followed by complement and that RDP not set
    nak();
    return;
  }
  ack();

  uint32_t addr;
  uint8_t chksm = 0;
  addr = recvAddr(&chksm);
  uint8_t host_chksm = recvByte();
  if (!inRange(addr, CMD_GO) || host_chksm != chksm) {
    nak();
    return;
  }

  ack();

  bootToUser(addr);

}

void writeMem() {
  if (0/* || FLASH_OB_GetRDP() == SET*/) {
    // chk if cmd is followed by complement and that RDP not set
    nak();
    return;
  }

  ack(); // ack that command is complemented and RDP not active

  uint32_t addr;
  uint8_t chksm = 0;
  addr = recvAddr(&chksm);
  uint8_t host_chksm = recvByte();
  if (!inRange(addr, CMD_WRITEMEM) || host_chksm != chksm) {
    nak();
    return;
  }

  ack(); // ack address and chksm ok

  uint8_t count = recvByte();
  chksm = count; // 07
  uint32_t i;
  uint8_t buf[256];
  for (i = 0; i <= count; i++) {
    buf[i] = recvByte();
    chksm ^= buf[i];
  }

  host_chksm = recvByte();

  if (chksm != host_chksm) {
    nak();
    return;
  }

  ack(); // ack the chksm

  for (i = count + 1; i < 256; i++) {
    buf[i] = 0xFF; // padding
  }

  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
  uint32_t pageStart = addrPageStart(addr); // pages in mults of 0x800
  if (pageStart == addr) FLASH_ErasePage(pageStart);
  if (addrPageStart(addr + count + 1) != pageStart) {
    FLASH_ErasePage(pageStart + PAGE_SIZE);
  }

  for (i = 0; i < 256; i+= 4) {
    uint32_t word = *(uint32_t *)&buf[i];
    // take addr of first byte in each word, cast as ptr to uint32_t, and deref
    FLASH_ProgramWord(addr + i, word);
    // addr + i in increments of 4 since we are programming by word
  }

  FLASH_Lock();
}

void erase() {
  if (FLASH_OB_GetRDP() != SET) {
    // chk if cmd is followed by complement and that RDP not set
    nak();
    return;
  }
}

void writeP() {
  if (FLASH_OB_GetRDP() != SET) {
    // chk if cmd is followed by complement and that RDP not set
    nak();
    return;
  }
}

void writeU() {
  if (FLASH_OB_GetRDP() != SET) {
    // chk if cmd is followed by complement and that RDP not set
    nak();
    return;
  }

  ack();
  FLASH_Unlock();
  ack();

  NVIC_SystemReset();
}

void readP() {
  if (FLASH_OB_GetRDP() != SET) {
    // chk if cmd is followed by complement and that RDP not set
    nak();
    return;
  }

  ack();
  FLASH_OB_Unlock();
  if (FLASH_OB_RDPConfig(OB_RDP_Level_1) != FLASH_COMPLETE) {
    nak();
    return;
  };
  FLASH_OB_Launch();
  FLASH_OB_Lock();

  ack();

  NVIC_SystemReset();
  f3d_uart_init();

}

void readU() {

  ack();

  FLASH_OB_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
  if (FLASH_OB_RDPConfig(OB_RDP_Level_0) != FLASH_COMPLETE) {
    nak();
    return;
  }
  FLASH_OB_Launch();
  FLASH_OB_Lock();

  // disable RDP
  ack();

  if (eraseRAM() != 1) {
    return;
  }

  NVIC_SystemReset();
  f3d_uart_init();
}

void test() {
  if (!chkCmdComp(TEST)) {
    nak();
    return;
  }
  int i;
  for (i = 0; i < 5; i++) {
    uint8_t buf = readFlash8(APPLICATION_ADDRESS + i);
    sendByte(buf);
  }
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);
  FLASH_ErasePage(APPLICATION_ADDRESS);

  FLASH_ProgramWord(APPLICATION_ADDRESS, 0xDEADBEEF);
  // writes in reverse byte order, so we have efbeadde as app address

  ack();
  for (i = 0; i < 5; i++) {
    uint8_t buf = readFlash8(APPLICATION_ADDRESS + i);
    sendByte(buf);
  }
}

void catchFunction() {
  while(1) {
    sendByte(0xff);
  }
}