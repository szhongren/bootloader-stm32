CC = gcc
CFLAGS = -g -Wall

default: loader

loader: loader.o host_utils.o host_cmd.o
	$(CC) $(CFLAGS) -o loader loader.o host_utils.o host_cmd.o

loader.o: loader.c host_cmd.h host_utils.h
	$(CC) $(CFLAGS) -c loader.c

host_cmd.o: host_cmd.c host_cmd.h
	$(CC) $(CFLAGS) -c host_cmd.c

host_utils.o: host_utils.c host_utils.h
	$(CC) $(CFLAGS) -c host_utils.c

