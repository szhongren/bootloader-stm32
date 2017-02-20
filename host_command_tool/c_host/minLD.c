#include "host_cmd.h"
#include "host_utils.h"

#define CMD_BUF_SIZE 64
#define DELIMS " \t\r\n"

int main(int argc, char *argv[]) {
  welcome();
  usage();

  char device[] = "/dev/ttyUSB0";
  unsigned int baud = 38400;
  int open = 0, port;
    
  char *cmd;
  char input[CMD_BUF_SIZE];
  
  while (1) {
    prompt();
    if (!fgets(input, CMD_BUF_SIZE, stdin)) break;

    if ((cmd = strtok(input, DELIMS))) {
      errno = 0;
      if (strcmp(cmd, "s") == 0 || strcmp(cmd, "start") == 0) {
        if (open) {
          printf("Port already open! Close it first if you want to reopen or call another command.\n");
        } else {
          port = ser_open(device);
          if (port != -1) {
            printf("Setting up port to %s.\n", device);
            ser_setup(device, port, baud);

            INIT(port, &open);
          }
        }
      
      } else if (strcmp(cmd, "h") == 0 || strcmp(cmd, "help") == 0) {
        char *helpwith = strtok(NULL, DELIMS);
        help(helpwith);
      
      } else if (strcmp(cmd, "w") == 0 || strcmp(cmd, "write") == 0) {
        char *addr = strtok(NULL, DELIMS);
        char *file = strtok(NULL, DELIMS);
        writeFile(port, addr, file, open);
      
      } else if (strcmp(cmd, "r") == 0 || strcmp(cmd, "read") == 0) {
        char *addr = strtok(NULL, DELIMS);
        char *bytes = strtok(NULL, DELIMS);
        CMD_READ(port, addr, bytes, open);
      
      } else if (strcmp(cmd, "g") == 0 || strcmp(cmd, "go") == 0) {
        char *addr = strtok(NULL, DELIMS);
        CMD_JUMP(port, addr, open);
      
      } else if (strcmp(cmd, "p") == 0 || strcmp(cmd, "protect") == 0) {
        char *rw = strtok(NULL, DELIMS);
        protect(port, rw, open);
      
      } else if (strcmp(cmd, "u") == 0 || strcmp(cmd, "unprotect") == 0) {
        char *rw = strtok(NULL, DELIMS);
        unprotect(port, rw, open);
      
      } else if (strcmp(cmd, "q") == 0 || strcmp(cmd, "quit") == 0) {
        if (open) close(port);
        printf("Exiting.\n");
        break;
      
      } else if (strcmp(cmd, "ds") == 0 || strcmp(cmd, "send") == 0) {
        char *byte = strtok(NULL, DELIMS);
        debugSend(port, byte, open);
        
      } else if (strcmp(cmd, "dr") == 0 || strcmp(cmd, "recv") == 0) {
        debugRecv(port, open);
        
      } else if (strcmp(cmd, "c") == 0 || strcmp(cmd, "close") == 0) {
        if (open) {
          open = 0;
          close(port);
          printf("Closing port to %s.\n", device);
        } else {
          printf("Port not open. Please use s|start to open the port first.\n");          
        }
      
      } else if (strcmp(cmd, "o") == 0 || strcmp(cmd, "options") == 0) {
        char *opt = strtok(NULL, DELIMS);
        char *new = strtok(NULL, DELIMS);
        options(opt, new, device, &baud);
        
      } else {
        printf("Unrecognized command: %s\n", cmd);
        usage();
      }
      if (errno) perror("Command failed");
    }
  }

  return 0;
}
