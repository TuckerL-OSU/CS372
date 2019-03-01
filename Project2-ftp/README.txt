Tucker Lavell
CS372 - Winter 2019
Project 2 - FTP

This program was tested to work on Flip2 and 3.

This program will work, whether or not server and client are in separate directories.
Compile:
	If they are in the same directory: type make in the command line
	If they are in separate directories: 
		Server: "gcc -o ftserver ftserver.c"
		Client: "chmod +x ftclient.py"

Commands to run:
Server:
	From the directory it was compiled: "./ftserver [port]"

Client:
	From the /Project2/Client folder: 
		Request List: "ftclient.py [server-addr] [port-on-server] -l [client-data-port]"

		Request File: "ftclient.py [server-addr] [port-on-server] -g [filename] [client-data-port]"

***IMPORTANT***
Server must be running before the client can make a connection.
Port chosen for either part needs to be ABOVE 10000.
The file to read from MUST be in the same directory as the server.
The file the client creates from requesting file can be found in the same directory as ftclient.py.
These can be run on the same server, as long as client-data-port is not the same as port-on-server.
If these are on different servers, client-data-port can be the same port as port-on-server.
If these are run from the same directory, client still works properly, but IT WILL OVERWRITE the original file.