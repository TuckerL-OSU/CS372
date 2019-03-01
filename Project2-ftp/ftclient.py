#!/bin/python

# Tucker Lavell
# CS372 - Winter 2019
# Project 2 - ftclient.py
# https://docs.python.org/2/howto/sockets.html
# 

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
    server_addr = sys.argv[1]        #We know the full path of flip. This will only work on flip
   
    server_port = int(sys.argv[2])                                       #Get the port number
    client_socket = socket(AF_INET,SOCK_STREAM)              #New socket
    
    client_socket.connect((server_addr, server_port))              #Call connect on it with the proper server name and server port
    
    return client_socket
    

# https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib
def getMyIP():
    ip = socket(AF_INET, SOCK_DGRAM)
    ip.connect(("8.8.8.8", 80))
    return ip.getsockname()[0]


def getFile(data_socket):                   #Get a file
    fd = open(sys.argv[4], "w")    
    buffer = data_socket.recv(1000)
    
    while "__done__" not in buffer:         #We specified in ftserver.c that once we were finished, we would send the done message. We need to keep going until we get it
        fd.write(buffer)
        buffer = data_socket.recv(1000)
        
        
def getList(data_socket):
    filename = data_socket.recv(100)     #Similar here. Loop until we hit done
    
    while filename != "done":        
        print(filename)
        filename = data_socket.recv(100)


def talkToServer(client_socket): 
    if sys.argv[3] == "-g":
        print("Reqesting file {}".format(sys.argv[4]))   #We know where the port number is based on the command
        port = 5
    elif sys.argv[3] == "-l":
        print("Requesting list.")
        port = 4

	# send port number
    client_socket.send(sys.argv[port])  
    
    client_socket.recv(1024)
    
    if sys.argv[3] == "-g":
        client_socket.send("g")
    else:
        client_socket.send("l")
    
    client_socket.recv(1024)
    
	# send IP, this is just to make it look nice on the server, not needed.
    client_socket.send(getMyIP())
    response = client_socket.recv(1024)
    
    if response == "invalid":                                                       #The server received an invalid command so inform the user and exit
        print("An invalid command was sent to the server.")
        exit(1)
    
    if sys.argv[3] == "-g":
        client_socket.send(sys.argv[4])
        response = client_socket.recv(1024)
        if response != "File found":                            #We set up "File found" as the response in ftserver.c. So we know if that's not the response then something went wrong
            print("Server did not find the file.'")
            return
    
	# open the data socket for the server to send its response
    data_socket = createSocket()

    if(sys.argv[3] == "-g"):        #Get a file
        getFile(data_socket)
        
    elif sys.argv[3] == "-l":       #Get directory list
        getList(data_socket)
   
    data_socket.close()             #We need to close this


if __name__ == "__main__":    
                                                                                                        #First we make sure user put the right number of arguments
    if len(sys.argv) < 5 or len(sys.argv) > 6:
        print("You must specify either five or six arguments.")
        exit(1)
        
    #elif (sys.argv[1] != "flip1" and sys.argv[1] != "flip2" and sys.argv[1] != "flip3"):    #The first argument must be the server        
    #    print("Please enter correct server name.")
    #    exit(1)
        
    elif (int(sys.argv[2]) < 1024 or int(sys.argv[2]) > 65535):             #The next argument needs to be a proper port                   
        print("Please enter a port number in the range 1024-65535")
        exit(1)
        
    elif (sys.argv[3] != "-g" and sys.argv[3] != "-l"):                     #Make sure the command is correct
        print("Invalid command. You must specify either -g or -l")
        exit(1)
        
    elif (sys.argv[3] == "-l" and (int(sys.argv[4])  < 1024 or int(sys.argv[4]) > 65535)):    #Make sure the port is correct    
        print("Please enter correct port number.")
        exit(1)
        
    elif (sys.argv[3] == "-g" and (int(sys.argv[5]) < 1024 or int(sys.argv[5]) > 65535)):        
        print("Please enter correct port number.")
        exit(1)
    
    client_socket = estConnection()  #Then we create a new socket
    
    talkToServer(client_socket)           #And finally we do what we need to here      