import sys 
from socket import *

# main
if __name__ == "__main__":
	#command line args
	if len(sys.argv) != 2:
		print "Incorrect Arguments. Try python chatserve.py [port]"
		exit (1)
