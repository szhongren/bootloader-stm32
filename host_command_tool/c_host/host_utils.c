#include "host_utils.h"

// Helper function: get baud ID from actual baud rate
#define BAUDCASE(x)  case x: return B##x
uint32_t ser_baud_to_id(uint32_t baud) {
  switch( baud ) {
    BAUDCASE( 1200 );
    BAUDCASE( 1800 );
    BAUDCASE( 2400 );
    BAUDCASE( 4800 );
    BAUDCASE( 9600 );
    BAUDCASE( 19200 );
    BAUDCASE( 38400 );
    BAUDCASE( 57600 );
    BAUDCASE( 115200 );
    BAUDCASE( 230400 );
  }
  return 0;
}

void welcome() {
  printf("********************************************************************************\n");
  printf("*                                                                              *\n");
  printf("*                           stm32 host flash loader                            *\n");
  printf("*                                                                              *\n");
  printf("********************************************************************************\n");
}

void usage() {
  printf("Available commands:\n");
  printf("\t- s|start\n");
  printf("\t- h|help <command>\n");
  printf("\t- w|write <address> <filename>\n");
  printf("\t- r|read <address> <bytes>\n");
  printf("\t- g|go <address>\n");
  printf("\t- p|protect <r|w>\n");
  printf("\t- u|unprotect <r|w>\n");
  printf("\t- c|close\n");
  printf("\t- q|quit\n");
  printf("Use h|help <command> to find out more about a command.\n");
}

void help(char *cmd) {
  if (cmd == NULL) {
    usage();
  } else {
    if (strcmp(cmd, "s") == 0 || strcmp(cmd, "start") == 0) {
      printf("s|start\n");
      printf("\t- Opens and sets up the serial port.\n");
    } else if (strcmp(cmd, "w") == 0 || strcmp(cmd, "write") == 0) {
      printf("w|write <address> <filename>\n");
      printf("\t- Writes the specified file to the specified address.\n");
      printf("\t- Address must be in hex form, maximum length of 8 hex digits. It can also be the words \"flash,\" \"sram,\" or \"user.\"\n");
    } else if (strcmp(cmd, "r") == 0 || strcmp(cmd, "read") == 0) {
      printf("r|read <address> <bytes>\n");
      printf("\t- Reads the specified number of bytes from the specified address.\n");
      printf("\t- Address must be in hex form, maximum length of 8 hex digits. It can also be the words \"flash,\" \"sram,\" or \"user.\"\n");
      printf("\t- Bytes must be from 1 to 256 inclusive, in decimal form.\n");
    } else if (strcmp(cmd, "g") == 0 || strcmp(cmd, "go") == 0) {
      printf("g|go <address>\n");
      printf("\t- Go or jump to the code at the given address.\n");
      printf("\t- Address must be in hex form, maximum length of 8 hex digits. It can also be the words \"flash,\" \"sram,\" or \"user.\"\n");
    } else if (strcmp(cmd, "p") == 0 || strcmp(cmd, "protect") == 0) {
      printf("p|protect <r|w>\n");
      printf("\t- Enables the read(r) or write(w) protection on the flash memory.\n");
    } else if (strcmp(cmd, "u") == 0 || strcmp(cmd, "unprotect") == 0) {
      printf("u|unprotect <r|w>\n");
      printf("\t- Disables the read(r) or write(w) protection on the flash memory.\n");
    } else if (strcmp(cmd, "c") == 0 || strcmp(cmd, "close") == 0) {
      printf("c|close\n");
      printf("\t- Closes the serial port.\n");
    } else if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0) {
      printf("q|quit\n");
      printf("\t- This is self explanatory.\n");
    } else {
      printf("Unrecognized command: %s\n", cmd);
      usage();
    }
  }
}

void prompt() {
  printf("> ");
}

int ser_open(const char device[]) {
  int port;
  printf("Attempting to open port at %s\n", device);
  if( ( port = open( device, O_RDWR | O_NOCTTY) ) == -1 )
    perror("ser_open: unable to open port");
  else {
    printf("Port to %s opened.\n", device);
    
    int val = fcntl(port, F_GETFL, 0);
    val &= ~FNDELAY;
    fcntl(port, F_SETFL, val);
    // fcntl( port, F_SETFL, 0 );
  }
  return port;
}

void ser_setup(char device[], int port, uint32_t baud) {
  // setup connection
  struct termios tio;
  
  usleep(200000);
  tcgetattr(port, &tio);

  tio.c_cflag &= ~CSTOPB; // 1 stop bit
  tio.c_cflag &= ~PARENB; // no parity check

  // Data bits
  tio.c_cflag |= ( CLOCAL | CREAD );
  tio.c_cflag &= ~CSIZE;
  tio.c_cflag |= CS8;


  tio.c_cflag &= ~CRTSCTS;
  tio.c_iflag &= ~( IXON | IXOFF | IXANY );

  // Raw input
  tio.c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG );

  tio.c_oflag &= ~OPOST;
  
  tio.c_iflag &= ~( INPCK | ISTRIP );

  tio.c_cc[ VMIN ] = 0;
  tio.c_cc[ VTIME ] = 1;

  cfsetospeed(&tio, ser_baud_to_id(baud));
  cfsetispeed(&tio, ser_baud_to_id(baud));

  tcsetattr(port, TCSANOW, &tio);

  // Flush everything
  tcflush(port, TCIOFLUSH);

  // unset fndelay
  int val = fcntl(port, F_GETFL, 0);
  val &= ~FNDELAY;
  fcntl(port, F_SETFL, val);
  // fcntl(port, F_SETFL, 0);
  
}

void debugSend(int port, char *hex, int open) {
  if (!open) {
    printf("Port is not open. Please use s|start to open the port first.\n");
    return;  
  }
  
  long byte = strtol(hex, NULL, 16);
  if (byte < 0x00 || byte > 0xff) {
    printf("Out of range! Can only send a byte, from 0x00 to 0xFF.\n");
    return;
  }
  printf("sending byte: %02x\n", (uint8_t)byte);
  sendByte(port, byte);
}

void debugRecv(int port, int open) {
  if (!open) {
    printf("Port is not open. Please use s|start to open the port first.\n");
    return;  
  }
  
  char recv;
  int res = read(port, &recv, 1);
  if (res == 1) {
    printf("received byte: %02x\n", (uint8_t)recv);
  } else {
    printf("Did not receive a byte.\n");
  }  
}

int sendByte(int port, uint8_t byte) {
  return write(port, &byte, 1);
}

int recvByte(int port) {
  char data;
  int res = read(port, &data, 1);
  return res == 1 ? data : -1;
}

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    fprintf(stderr, "Cannot determine size of %s: %s\n", filename, strerror(errno));

    return -1; 
}

uint32_t revWord(uint32_t Word) {
  uint32_t rev = 0;
  rev |= (Word & 0x000000FF) << 24;
  rev |= (Word & 0x0000FF00) << 8;
  rev |= (Word & 0x00FF0000) >> 8;
  rev |= (Word & 0xFF000000) >> 24;
  return rev;
}

uint16_t revHalfWord(uint16_t hWord) {
  uint16_t rev = 0;
  rev |= (hWord & 0x00FF) << 8;
  rev |= (hWord & 0xFF00) >> 8;
  return rev;
}
