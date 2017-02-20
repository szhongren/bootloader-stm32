#!/usr/bin/python

import sys, getopt
from serial import *
from base64 import *
from struct import *

def usage():
    print "./minLD.py -w <filename> -p <r|w> -u <r|w>"

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "hw:p:u:", ["help", "write=", "protect=", "unprotect="])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt in ("-w", "--write"):
            print "writing %s to board" % arg
        elif opt in ("-p", "--protect"):
            print "protecting %s" % arg
        elif opt in ("-u", "--unprotect"):
            print "unprotecting %s" % arg

if __name__ == "__main__":
    main(sys.argv[1:])
