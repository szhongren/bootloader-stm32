CC = gcc
CFLAGS = -g -Wall

default: minLD

minLD: minLD.o host_utils.o host_cmd.o
	$(CC) $(CFLAGS) -o minLD minLD.o host_utils.o host_cmd.o

minLD.o: minLD.c host_cmd.h host_utils.h
	$(CC) $(CFLAGS) -c minLD.c

host_cmd.o: host_cmd.c host_cmd.h
	$(CC) $(CFLAGS) -c host_cmd.c

host_utils.o: host_utils.c host_utils.h
	$(CC) $(CFLAGS) -c host_utils.c

