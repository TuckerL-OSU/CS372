Tucker Lavell
CS372 - Winter 2019
Project 2 - FTP

This program was tested to work on all Flip servers.

This program will work, whether or not server and client are in separate directories.
However, there can be strange behavior if they are in the same directory.
Compile:
	If they are in the same directory: type make in the command line
	If they are in separate directories: 
		Server: "gcc -o ftserver ftserver.c"
		Client: "chmod +x ftclient.py"

Commands to run:
Server:
	From the directory it was compiled: "./ftserver [port]"

Client:
	From the directory it was compiled: 
		Request List: "ftclient.py [server-addr] [port-on-server] -l [client-data-port]"

		Request File: "ftclient.py [server-addr] [port-on-server] -g [filename] [client-data-port]"

***IMPORTANT***
Server must be running before the client can make a connection.
Port chosen for either part needs to be ABOVE 10000.
In client, Server Address can be in the form "flipX" if tested on flip. 
	Other connections also work, with the full IP/URL.
The file to read from MUST be in the same directory as the server.
The file the client creates from requesting file can be found in the same directory as ftclient.py.
These can be run on the same server, as long as client-data-port is not the same as port-on-server.
If these are on different servers, client-data-port can be the same port as port-on-server.
If these are run from the same directory, client still works properly, but IT WILL OVERWRITE the original file.
Give client up to 1 minute to finalize the file transfer.
