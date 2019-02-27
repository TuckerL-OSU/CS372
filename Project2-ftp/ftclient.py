#!/bin/python

import sys
from socket import *

def createSocket():    
    if sys.argv[3] == "-l":
        numArguments = 4
    elif sys.argv[3] == "-g":                               #This was helpful https://docs.python.org/2/howto/sockets.html
        numArguments = 5

    serverport = int(sys.argv[numArguments])
    serversocket = socket(AF_INET, SOCK_STREAM)
    serversocket.bind(('', serverport))
    serversocket.listen(1)
    dataSocket, address = serversocket.accept()
    return dataSocket

    
def connectServer():                                            #Helpful again https://docs.python.org/2/howto/sockets.html   
    serverName = sys.argv[1]+".engr.oregonstate.edu"        #We know the full path of flip. This will only work on flip
   
    serverPort = int(sys.argv[2])                                       #Get the port number
    clientSocket = socket(AF_INET,SOCK_STREAM)              #New socket
    
    clientSocket.connect((serverName, serverPort))              #Call connect on it with the proper server name and server port
    
    return clientSocket
    
def getIP():
    s = socket(AF_INET, SOCK_DGRAM)     #Easy way to get IP address
    s.connect(("8.8.8.8", 80))
    return s.getsockname()[0]

def getFile(dataSocket):                   #Get a file
    f = open(sys.argv[4], "w")    
    buffer = dataSocket.recv(1000)
    
    while "__done__" not in buffer:         #We specified in ftserver.c that once we were finished, we would send the done message. We need to keep going until we get it
        f.write(buffer)
        buffer = dataSocket.recv(1000)
        
        
def getList(dataSocket):
    filename = dataSocket.recv(100)     #Similar here. Loop until we hit done
    
    while filename != "done":        
        print filename
        filename = dataSocket.recv(100)



def getData(clientSocket): 
    if sys.argv[3] == "-g":
        print "Reqesting file {}".format(sys.argv[4])   #We know where the port number is based on the command
        portnumber = 5
    elif sys.argv[3] == "-l":
        print "Requesting list."
        portnumber = 4
    
    clientSocket.send(sys.argv[portnumber])     #Send port number
    
    clientSocket.recv(1024)
    
    if sys.argv[3] == "-g":
        clientSocket.send("g")
    else:
        clientSocket.send("l")
    
    clientSocket.recv(1024)
    
    clientSocket.send(getIP())                  #Send IP address
    response = clientSocket.recv(1024)
    
    if response == "bad":                                                       #The server received an invalid command so inform the user and exit
        print "An invalid command was sent to the server."
        exit(1)
    
    if sys.argv[3] == "-g":
        clientSocket.send(sys.argv[4])
        response = clientSocket.recv(1024)
        if response != "File found":                            #We set up "File found" as the response in ftserver.c. So we know if that's not the response then something went wrong
            print "Server did not find the file.'"
            return
    
    dataSocket = createSocket()

    if(sys.argv[3] == "-g"):        #Get a gile
        getFile(dataSocket)
        
    elif sys.argv[3] == "-l":       #Get directory list
        getList(dataSocket)

    
    dataSocket.close()             #We need to close this




if __name__ == "__main__":    
                                                                                                        #First we make sure user put the right number of arguments
    if len(sys.argv) < 5 or len(sys.argv) > 6:
        print "You must specify either five or six arguments."
        exit(1)
        
    elif (sys.argv[1] != "flip1" and sys.argv[1] != "flip2" and sys.argv[1] != "flip3"):    #The first argument must be the server        
        print "Please enter correct server name."
        exit(1)
        
    elif (int(sys.argv[2]) < 1024 or int(sys.argv[2]) > 65535):             #The next argument needs to be a proper port                   
        print "Please enter a port number in the range 1024-65535"
        exit(1)
        
    elif (sys.argv[3] != "-g" and sys.argv[3] != "-l"):                     #Make sure the command is correct
        print "Invalid command. You must specify either -g or -l"
        exit(1)
        
    elif (sys.argv[3] == "-l" and (int(sys.argv[4])  < 1024 or int(sys.argv[4]) > 65535)):    #Make sure the port is correct    
        print "Please enter correct port number."
        exit(1)
        
    elif (sys.argv[3] == "-g" and (int(sys.argv[5]) < 1024 or int(sys.argv[5]) > 65535)):        
        print "Please enter correct port number."
        exit(1)
    
    clientSocket = connectServer()  #Then we create a new socket
    
    
    getData(clientSocket)           #And finally we do what we need to here      