#!/bin/python

# Tucker Lavell
# CS372 - Winter 2019
# Project 2 - ftclient.py
# Most help taken from https://docs.python.org/2/howto/sockets.html and
# https://realpython.com/python-sockets/ 
# Help recieved from friend Dan Drapp
# Noted within is extra help I needed/found.

import sys
from socket import *

def createSocket():    
    if sys.argv[3] == "-l":
        numArguments = 4
    elif sys.argv[3] == "-g":
        numArguments = 5

    client_port = int(sys.argv[numArguments])
    sockfd = socket(AF_INET, SOCK_STREAM)
    sockfd.bind(('', client_port))
    sockfd.listen(0)
    data_socket, address = sockfd.accept()
    return data_socket


def estConnection():
	# server to connect to address
    server_addr = sys.argv[1] 
   
	# port to connect to on the server
    server_port = int(sys.argv[2])
    client_socket = socket(AF_INET,SOCK_STREAM)
    client_socket.connect((server_addr, server_port))
    
    return client_socket
    

# https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib
def getMyIP():
    ip = socket(AF_INET, SOCK_DGRAM)
    ip.connect(("1.1.1.1", 80))
    return ip.getsockname()[0]


def fileHandler(data_socket):
    fd = open(sys.argv[4], "w+")   
    buffer = data_socket.recv(1000)
    
	# keep ingesting file while end of transfer hasn't been hit
    while "end_of_file" not in buffer:
        fd.write(buffer)
        buffer = data_socket.recv(1000)
        
        
def getList(data_socket):
    filename = data_socket.recv(100)
    
	# keep ingesting file while end of transfer hasn't been hit
    while filename != "end_of_list":        
        print(filename)
        filename = data_socket.recv(100)


def talkToServer(client_socket): 
    # get port based off of command 
	if sys.argv[3] == "-g": 
		print("Reqesting file: {}".format(sys.argv[4]))
		port = 5
	elif sys.argv[3] == "-l":
		print("Requesting list.")
		port = 4

	# send port number
	client_socket.send(sys.argv[port])  
	
	client_socket.recv(1024)
	
	# send command
	if sys.argv[3] == "-g":
		client_socket.send("g")
	else:
		client_socket.send("l")
	
	client_socket.recv(1024)
	
	# send IP, this is just to make it look nice on the server, not needed.
	client_socket.send(getMyIP())
	response = client_socket.recv(1024)
	
	if response == "invalid":
		print("An invalid command was sent to the server.")
		exit(1)
	
	if sys.argv[3] == "-g":
		client_socket.send(sys.argv[4])
		response = client_socket.recv(1024)
		# check if the server found the file we requested
		if response != "File found": 
			print("Server couldn't find {}.".format(sys.argv[4]))
			return
	
	# open the data socket for the server to send its response
	data_socket = createSocket()

	# get the file
	if(sys.argv[3] == "-g"):
		fileHandler(data_socket)
	
	# get the list of files in the servers current directory
	elif sys.argv[3] == "-l":
		getList(data_socket)
	
	data_socket.close()


if __name__ == "__main__":
	# light input validation
	if len(sys.argv) < 5 or len(sys.argv) > 6:
		print("You must specify either five or six arguments.")
		exit(1)
		
	elif (int(sys.argv[2]) < 10000 or int(sys.argv[2]) > 65535):
		print("Please enter a port number in the range 10000-65535.")
		exit(1)
		
	elif (sys.argv[3] != "-g" and sys.argv[3] != "-l"):
		print("Invalid command. You must specify either -g or -l")
		exit(1)
		
	elif (sys.argv[3] == "-l" and (int(sys.argv[4])  < 10000 or int(sys.argv[4]) > 65535)):
		exit(1)
		
	elif (sys.argv[3] == "-g" and (int(sys.argv[5]) < 10000 or int(sys.argv[5]) > 65535)):        
		print("Please enter a port number in the range 10000-65535.")
		exit(1)
	
	client_socket = estConnection()
	
	talkToServer(client_socket)