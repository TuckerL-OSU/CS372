Tucker Lavell
CS372 - Winter 2019
Project 2 - FTP

This program was tested to work on Flip2 and 3.

To compile just type make in the command line, while inside the Project2 folder.
The file to read also needs to be in this same directory /Project2/

Commands to run:
Server:
	From the /Project2/ folder: "./ftserver [port]""

Client:
	From the /Project2/Client folder: 
		Request List: "ftclient.py [server-addr] [port-on-server] -l [client-data-port]"

		Request File: "ftclient.py [server-addr] [port-on-server] -g [filename] [client-data-port]"

***IMPORTANT***
Server must be running before the client can make a connection.
Port chosen for either part needs to be ABOVE 10000.
The file to read from MUST be in /Project2/ folder.
The file the client creates from requesting file can be found in /Project2/Client
These can be run on the same server, as long as client-data-port is not the same as port-on-server.
If these are on different servers, client-data-port can be the same port as port-on-server.