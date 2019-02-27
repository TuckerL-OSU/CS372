#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

//This program heavily references Beej's guide and I will point out when/where 
//There are also some reused functions from Assignment 1


struct addrinfo * createAddress(char* port) {   //This function ia used to create the address information
	int status;
	struct addrinfo hints;
	struct addrinfo * res;

	memset(&hints, 0, sizeof hints);        //Taken right from Beej's guide, empty the struct
	hints.ai_family = AF_INET;                  //Used to specify version
	hints.ai_socktype = SOCK_STREAM;        //TCP stream sockets
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {       //Also from Beej's guide. Print error if status isn't 0
		fprintf(stderr,
			"Error. Please enter the correct port.\n",
			gai_strerror(status));
		exit(1);
	}

	return res;
}

struct addrinfo * createAddressIP(char* ipAddress, char* port) {      //Creating the address info. Also from Beej's guide
	int status;
	struct addrinfo hints;
	struct addrinfo * res;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(ipAddress, port, &hints, &res)) != 0) {
		fprintf(stderr,
			"Error. Please enter the correct port\n",
			gai_strerror(status));
		exit(1);
	}

	return res;
}


int createSocket(struct addrinfo * res) {               //The next step in Beej's guide after creating the address info. We want to feed that into the socket
	int sockfd;                                                     //It's either going to return a socket descriptor to be used later or it will return -1 and exit with a error
	if ((sockfd = socket((struct addrinfo *)(res)->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		fprintf(stderr, "Error. Socket was not created.\n");
		exit(1);
	}
	return sockfd;
}


void connectSocket(int sockfd, struct addrinfo * res) {             //What we will use for the conneciton itself. Very similar to the creation
	int status;
	if ((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == -1) {
		fprintf(stderr, "Error in connection.\n");
		exit(1);
	}
}


void bindSocket(int sockfd, struct addrinfo * res) {                //Now we need to bind the socket. Also from Beej's guide
	if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		close(sockfd);
		fprintf(stderr, "Error in binding socket\n");
		exit(1);
	}
}

void listenSocket(int sockfd) {                     //Now that it's bound, we need to listen
	if (listen(sockfd, 5) == -1) {
		close(sockfd);
		fprintf(stderr, "Error. Cannot listen.\n");
		exit(1);
	}
}

int getFiles(char** files) {         //We need to get all of the files in the directory
	DIR* d;                                 //Structure for this function was found here: https://goo.gl/oqbjTv
	struct dirent * dir;
	d = opendir(".");
	int i = 0;
	if (d) {
		while ((dir = readdir(d)) != NULL) {     //While there are still things to read, read in the file names
			if (dir->d_type == DT_REG) {
				strcpy(files[i], dir->d_name);
				i++;
			}
		}
		closedir(d);
	}
	return i;
}


char ** createStringHold(int size) {                         //This is used to hold directory files
	char ** array = malloc(size * sizeof(char *));
	int i;
	for (i = 0; i < size; i++) {
		array[i] = malloc(100 * sizeof(char));
		memset(array[i], 0, sizeof(array[i]));
	}
	return array;
}


void deleteStringHold(char ** array, int size) {     //Function to clear the array
	int i;
	for (i = 0; i < size; i++) {
		free(array[i]);
	}
	free(array);
}



int checkFile(char ** files, int numFiles, char * filename) {     //C style bool to see if the file is there
	int fileFound = 0;                //We assume the file is not there until our loop
	int i;

	for (i = 0; i < numFiles; i++) {             //Loop through all the files to check the name
		if (strcmp(files[i], filename) == 0) {
			fileFound = 1;
		}
	}
	return fileFound;             //This will have either been set to true (1) in the loop or false if not
}

void sendFile(char* ipAddress, char * port, char * filename) {                //This is what we'll use to send the file
	sleep(2);
	struct addrinfo * res = createAddressIP(ipAddress, port);      // Call our function to create the address
	int dataSocket = createSocket(res);                                               //Create the socket and then connect it. These are all functions we wrote from Beej's guide
	connectSocket(dataSocket, res);

	char buffer[2000];                                                                          //Create a buffer for the file and clear it with memset	
	memset(buffer, 0, sizeof(buffer));

	int fd = open(filename, O_RDONLY);                                                      //While loop to read the file. Only need to read it
	while (1) {
		int numBytes = read(fd, buffer, sizeof(buffer) - 1);                        //Get structure for this portion from here: https://goo.gl/X8b7sH
		if (numBytes == 0)
			break;

		if (numBytes < 0) {
			fprintf(stderr, "Error. Cannot read file.\n");
			return;
		}

		void* hold = buffer;
		while (numBytes > 0) {
			int numBytesWritten = send(dataSocket, hold, sizeof(buffer), 0);
			if (numBytesWritten < 0) {
				fprintf(stderr, "Error in writing\n");
				return;
			}
			numBytes -= numBytesWritten;
			hold += numBytesWritten;
		}

		memset(buffer, 0, sizeof(buffer));              //Clear this out
	}

	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "__done__");
	send(dataSocket, buffer, sizeof(buffer), 0);

	close(dataSocket);          //Per project specifications. Can't leave this open
	freeaddrinfo(res);
}

void sendDirectory(char * ipAddress, char * port, char ** files, int totalNum) {      //Function to send directory
	sleep(2);
	struct addrinfo * res = createAddressIP(ipAddress, port);      //Similar setup for connections
	int dataSocket = createSocket(res);
	connectSocket(dataSocket, res);

	int i;
	for (i = 0; i < totalNum; i++) {
		send(dataSocket, files[i], 100, 0);                 //Send for the total number of files
	}

	char* completed = "done";
	send(dataSocket, completed, strlen(completed), 0);

	close(dataSocket);
	freeaddrinfo(res);
}


void acceptConnection(int new_fd) {	            //Structure for this borrowed from Beej's guide
	char * good = "ok";
	char * bad = "bad";
	char port[100];
	char command[500];

	memset(port, 0, sizeof(port));              //Make sure to clear before send
	recv(new_fd, port, sizeof(port) - 1, 0);

	send(new_fd, good, strlen(good), 0);


	memset(command, 0, sizeof(command));            //Same here
	recv(new_fd, command, sizeof(command) - 1, 0);

	send(new_fd, good, strlen(good), 0);

	char ipAddress[100];
	memset(ipAddress, 0, sizeof(ipAddress));
	recv(new_fd, ipAddress, sizeof(ipAddress) - 1, 0);

	printf("New connection from %s\n", ipAddress);

	if (strcmp(command, "g") == 0) {              //They want a single file. 
		send(new_fd, good, strlen(good), 0);

		char filename[100];
		memset(filename, 0, sizeof(filename));
		recv(new_fd, filename, sizeof(filename) - 1, 0);
		printf("File: %s requested \n", filename);

		char** files = createStringHold(500);
		int numFiles = getFiles(files);         //Use the function to check if the file is there
		int findFile = checkFile(files, numFiles, filename);
		if (findFile) {

			printf("File found, sending %s to client\n", filename);
			char * file_found = "File found";
			send(new_fd, file_found, strlen(file_found), 0);

			char new_filename[100];
			memset(new_filename, 0, sizeof(new_filename));
			strcpy(new_filename, "./");
			char * end = new_filename + strlen(new_filename);
			end += sprintf(end, "%s", filename);

			sendFile(ipAddress, port, new_filename);
		}
		else {
			printf("Could not find file. Sending error to client.\n");
			char * notFound = "File not found";
			send(new_fd, notFound, 100, 0);
		}
		deleteStringHold(files, 500);
	}

	else if (strcmp(command, "l") == 0) {                  //Directory request so get the number of files and send them

		send(new_fd, good, strlen(good), 0);
		printf("File list requested \n");
		printf("Sending file list to %s \n", ipAddress);

		char** files = createStringHold(500);

		int numFiles = getFiles(files);

		sendDirectory(ipAddress, port, files, numFiles);

		deleteStringHold(files, 500);
	}
	else {
		send(new_fd, bad, strlen(bad), 0);
		printf("Got invalid command.\n");
	}

	printf("Waiting for more connections.\n");
}

void waitForConnection(int sockfd) {	                //This function to wait for more connections is also from Beej's guide
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	int new_fd;

	while (1) {
		addr_size = sizeof(their_addr);
		new_fd = accept(sockfd, (struct addrinfo *)&their_addr, &addr_size);

		if (new_fd == -1) {
			continue;
		}

		acceptConnection(new_fd);
		close(new_fd);
	}
}


int main(int argc, char *argv[]) {
	if (argc != 2) {                  //We already know the usage is the executable plus the port number
		fprintf(stderr, "Usage: ./ftserver [server]\n");
		exit(1);
	}

	printf("Server now listening on port %s\n", argv[1]);   //Now we just call all the functions we wrote in main
	struct addrinfo* res = createAddress(argv[1]);
	int sockfd = createSocket(res);
	bindSocket(sockfd, res);
	listenSocket(sockfd);
	waitForConnection(sockfd);
	freeaddrinfo(res);
}