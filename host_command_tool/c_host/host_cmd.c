#include "host_cmd.h"
#include "host_utils.h"

void SEND_CMD(int port, uint8_t cmd) {
  sendByte(port, cmd);
  sendByte(port, (~cmd)&0xff);
}

void INIT(int port, int* open) {
  printf("Sending init byte: %02x.\n", CMD_INIT);
  sendByte(port, CMD_INIT);

  if (expectACK(port, "Error in init. ACK not received.") == 1) {
    *open = 1;
    printf("ACK received. Port successfully init.\n");
    // successfully opened and set up
  }
}

void CMD_WRITE(int port, uint32_t addr, uint32_t sz, uint8_t buffer[]) {

  SEND_CMD(port, CMD_WRITEMEM);
  if (expectACK(port, "CMD not ACKED. Could be wrong complement or RDP active.") != 1) {
    return;
  }

  sendAddr(port, addr);
  if (expectACK(port, "Error sending address.") != 1) {
    return;
  }

  uint8_t nBytes = sz - 1;
  uint8_t xor = nBytes;
  sendByte(port, nBytes);

  int i;
  for (i = 0; i < sz; i+=8) {
    if (DEBUG) printf("0x%08x= ", addr + i);
    int x;
    for (x = 0; x < 8; x++) {
      if (i + x < sz) {
        sendByte(port, buffer[i + x]);
        xor ^= (buffer[i + x] & 0xff);
        if (DEBUG) printf("%02x  ", buffer[i + x] & 0xff);
      } else {
        if (DEBUG) printf("    ");
      }
    }
    for (x = 0; x < 8; x++) {
      if (i + x < sz) {
        if (DEBUG) printf("%c", isprint(buffer[i + x]) ? buffer[i + x] : '.');
      } else {
        if (DEBUG) printf(" ");
      }
    }
    printf("\n");
  } // better output
  /*
  for (i = 0; i < sz; i++) {
    sendByte(port, buffer[i]);
    if (DEBUG) {
      printf("0x%08x = 0x%02x", addr + i, buffer[i]);
      if (isprint(buffer[i]))
        printf(" - %c", buffer[i]);
      printf("\n");
    }
    xor ^= (buffer[i] & 0xff);
  }*/
  sendByte(port, xor);
  if (expectACK(port, "Error sending data.") != 1) {
    return;
  }
  printf("Wrote %d/0x%03x bytes starting at %08x.\n", sz, sz, addr);
}

void writeFile(int port, char* addr, char* file, int open) {
  if (!open) {
    printf("Port not open. Please use s|start to open the port first.\n");
    return;
  }
  uint32_t startAddr = parseAddr(addr);

  long sz = fsize(file); // 8 bytes
  if (sz == -1) {
    return;
  }
  printf("Writing %s (%ld/0x%04x bytes) to 0x%08x.\n", file, sz, sz, startAddr);

  FILE* pFile;
  uint8_t buffer[256];
  size_t result;

  pFile = fopen(file, "rb");
  if (pFile == NULL) {
    perror("File opening error.");
    return;
  }
  int cycles = sz / 256;
  int extra = sz % 256;
  int i;

  for (i = 0; i < cycles; i++) {
    result = fread(buffer, 1, 256, pFile);
    if (result != 256) {
      perror("File reading error.");
      return;
    }
    CMD_WRITE(port, startAddr + i * 256, 256, buffer);
    usleep(1000000);

  }

  if (extra > 0) {
    result = fread(buffer, 1, extra, pFile);
    if (result != extra) {
      perror("File reading error.");
      return;
    }
    CMD_WRITE(port, startAddr + i * 256, extra, buffer);
    usleep(1000000);
    // allow time for bootloader to finish writing
  }
  printf("Wrote %s (%ld/0x%04x bytes) from 0x%08x to 0x%08x.\n",
    file, sz, sz, startAddr, startAddr + sz + extra);
  fclose(pFile);
}

void CMD_READ(int port, char* addr, char* bytes, int open) {
  if (!open) {
    printf("Port not open. Please use s|start to open the port first.\n");
    return;
  }
  uint32_t startAddr = parseAddr(addr);
  uint32_t nBytes = atoi(bytes) - 1;
  if (nBytes < 0 || nBytes > 255) {
    printf("The bytes you entered, %d, was not a number or out of the range of 1 - 256.\n", nBytes + 1);
    return;
  }

  SEND_CMD(port, CMD_READMEM);
  if (expectACK(port, "Error in READMEM.") != 1) {
    return;
  }

  sendAddr(port, startAddr);
  if (expectACK(port, "Error sending address.") != 1) {
    return;
  }

  uint8_t xor = (~nBytes & 0xff);
  sendByte(port, nBytes);
  sendByte(port, xor);
  if (expectACK(port, "Error sending nBytes or checksum.") != 1) {
    return;
  }

  int i;
  char buffer[256];
  for (i = 0; i <= nBytes; i+=8) {
    if (DEBUG) printf("0x%08x= ", startAddr + i);
    int x;
    for (x = 0; x < 8; x++) {
      if (i + x <= nBytes) {
        buffer[i + x] = recvByte(port);
        if (DEBUG) printf("%02x  ", buffer[i + x] & 0xff);
      } else {
        if (DEBUG) printf("    ");
      }
    }
    for (x = 0; x < 8; x++) {
      if (i + x <= nBytes) {
        if (DEBUG) printf("%c", isprint(buffer[i + x]) ? buffer[i + x] : '.');
      } else {
        if (DEBUG) printf(" ");
      }
    }
    printf("\n");
  } // better output
  /*
  for (i = 0; i <= nBytes; i++) {
    uint8_t c = recvByte(port);
    printf("%08x = %02x", startAddr + i, c);
    if (isprint(c))
      printf(" - %c", c);
    printf("\n");
  }*/
  printf("Read %d/0x%03x bytes starting at %08x.\n", nBytes + 1, nBytes + 1, startAddr);

}

void CMD_JUMP(int port, char* addr, int open) {
  if (!open) {
    printf("Port not open. Please use s|start to open the port first.\n");
    return;
  }
  uint32_t jumpAddr = parseAddr(addr);

  SEND_CMD(port, CMD_GO);
  if (expectACK(port, "Error in JUMP.") != 1) {
    return;
  }

  sendAddr(port, jumpAddr);
  if (expectACK(port, "Error sending address.") != 1) {
    return;
  }
  printf("Jumping to user code at %08x.\n", jumpAddr);
}

void CMD_WRITEU(int port) {
  printf("WRITE UNPROTECT\n");
}

void CMD_WRITEP(int port) {
  printf("WRITE PROTECT\n");
}

void CMD_READU(int port) {
  printf("READ UNPROTECT\n");
}

void CMD_READP(int port) {
  printf("READ PROTECT\n");
}

void protect(int port, char* rw, int open) {
  if (!open) {
    printf("Port not open. Please use s|start to open the port first.\n");
    return;
  }

  if (rw == NULL) {
    printf("rw not specified\n");
  } else if (strcmp(rw, "r") == 0) {
    CMD_READP(port);
  } else if (strcmp(rw, "w") == 0) {
    CMD_WRITEP(port);
  } else {
    printf("unrecognized arg: %s\n", rw);
  }
}

void unprotect(int port, char* rw, int open) {
  if (!open) {
    printf("Port not open. Please use s|start to open the port first.\n");
    return;
  }

  if (rw == NULL) {
    printf("rw not specified\n");
  } else if (strcmp(rw, "r") == 0) {
    CMD_READU(port);
  } else if (strcmp(rw, "w") == 0) {
    CMD_WRITEU(port);
  } else {
    printf("unrecognized arg: %s\n", rw);
  }
}

void options(char* opt, char* new, char device[], unsigned int* baud) {
  if (opt == NULL) {
    printf("Device:\t%s\n", device);
    printf("Baud:\t%d\n", *baud);
  } else if (strcmp(opt, "d") == 0) {
    if (new == NULL) {
      printf("New device not specified.\n");
    } else {
      strcpy(device, new);
      printf("Device set to %s.\n", new);
    }
  } else if (strcmp(opt, "b") == 0) {
    if (new == NULL) {
      printf("New baud not specified.\n");
    } else {
      int newBaud = strtol(new, NULL, 10);
      if (newBaud == 50 || newBaud == 75 || newBaud == 110 || newBaud == 134 || newBaud == 150 || newBaud == 200 || newBaud == 300 || newBaud == 600 || newBaud == 1200 || newBaud == 1800 || newBaud == 2400 || newBaud == 4800 || newBaud == 9600 || newBaud == 19200 || newBaud == 38400 || newBaud == 57600 || newBaud == 115200) {
        *baud = newBaud;
        printf("Baud set to %d.\n", *baud);
      } else {
        printf("Invalid baud rate: %d.\n", newBaud);
      }
    }
  } else {
    printf("unrecognized arg: %s\n", opt);
  }
}

uint32_t parseAddr(char *addr) {
  if (strcmp(addr, "flash") == 0) {
    return FLASH_BEGIN;
  } else if (strcmp(addr, "sram") == 0) {
    return SRAM_BEGIN;
  } else if (strcmp(addr, "user") == 0) {
    return APPLICATION_ADDRESS;
  } else {
    uint32_t parsed = strtol(addr, NULL, 16);
    if (parsed < 0 || parsed > 0xFFFFFFFF) {
      printf("The address you entered: %s, was out of range or not a number. Defaulting to 0x%08x.\n", addr, APPLICATION_ADDRESS);
      return APPLICATION_ADDRESS;
    } else {
      return parsed;
    }
  }
}

void sendAddr(int port, uint32_t addr) {
  uint8_t b0 = (addr >> 24) & 0xff;
  uint8_t b1 = (addr >> 16) & 0xff;
  uint8_t b2 = (addr >> 8) & 0xff;
  uint8_t b3 = (addr >> 0) & 0xff;
  uint8_t xor = b0 ^ b1 ^ b2 ^ b3;

  sendByte(port, b0);
  sendByte(port, b1);
  sendByte(port, b2);
  sendByte(port, b3);
  sendByte(port, xor);
}

int expectACK(int port, char *msg) {
  int reply = recvByte(port);
  if (reply == ACK) {
    if (DEBUG) {
      printf("ACK--------------------------------------------------------------------------ACK\n");
    }
    return 1;
  } else if (reply == NAK) {
    if (DEBUG) {
      printf("NAK--------------------------------------------------------------------------NAK\n");
    }
    return 0;
  } else {
      printf("NO REPLY: %s\n", msg);
    return -1;
  }
}
