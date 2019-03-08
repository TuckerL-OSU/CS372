// Tucker Lavell
// CS372 - Winter 2019
// Project 2 - ftserver.c
//
// Much of the socket code was taken DIRECTLY from beej's code samples and suggestions.
// https://beej.us/guide/bgnet/html/single/bgnet.html
// I was also able to reuse some of my code from P1 and from a past class.
// Help recieved from friend Dan Drapp
// Noted within is extra help I needed/found.
// Also failed attempts to make the code more modular
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

#define MAX_CONNS 5

void error(int exitCode, const char *msg) {
	// use stderr to print error so we can set exit codes
	// for debugging and potential future use
	fprintf(stderr, "%s\n", msg);
	if (exitCode != 0) {
		exit(exitCode);
	}
}

// prepare port for connections
struct addrinfo *openConnection(char *port) {
	struct addrinfo hints;
	struct addrinfo *result;
	int status = 0;

	memset(&hints, 0, sizeof hints);
	// version
	hints.ai_family = AF_INET;
	// TCP stream
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port, &hints, &result)) != 0) {
		error(2, "Error opening port.\n");
	}

	return result;
}

// create the address structure sockets need to connect
struct addrinfo *createConnection(char *addr, char *port) {
	struct addrinfo hints;
	struct addrinfo *result;
	int status = 0;

	memset(&hints, 0, sizeof hints);
	// version
	hints.ai_family = AF_INET;
	// TCP stream
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(addr, port, &hints, &result)) != 0) {
		error(2, "Error getting address.\n");
	}

	return result;
}

int createSocket(struct addrinfo *conn) {
	int sockfd;

	if ((sockfd = socket(conn->ai_family, conn->ai_socktype, conn->ai_protocol)) == -1) {
		error(-1, "Failed to create socket.\n");
	}

	return sockfd;
}

void bindSocket(int sockfd, struct addrinfo *conn) {
	if (bind(sockfd, conn->ai_addr, conn->ai_addrlen) == -1) {
		close(sockfd);
		error(1, "Failed to bind socket.\n");
	}
}

void listenSocket(int sockfd) {
	if (listen(sockfd, MAX_CONNS) == -1) {
		close(sockfd);
		error(1, "Error listening to socket.\n");
	}
}

// establish a connection with the client
int estConnection(int sockfd, struct addrinfo *conn) {
	int status = 0;

	if ((status = connect(sockfd, conn->ai_addr, conn->ai_addrlen)) == -1) {
		error(0, "Failed to establish connection from Client.\n");
		close(sockfd);
	}

	return status;
}

// directory navigation: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
int getNumFilesInDir(char** files) {
	DIR* fd;
	struct dirent *dir;
	fd = opendir(".");
	int i = 0;
	if (fd) {
		while ((dir = readdir(fd)) != NULL) {
			if (/*!strcmp(dir->d_name, ".") == 0 ||*/ !strcmp(dir->d_name, "..") == 0) {
				strcpy(files[i], dir->d_name);
				i++;
			}
		}
		closedir(fd);
	}
	else {
		error(4, "Failed to find Directory.\n");
	}

	return i;
}

// container to store the file names
char **initContainer_filesInDir(int size) {
	char **filesInDir = malloc(size * sizeof(char *));

	int i;
	for (i = 0; i < size; i++) {
		filesInDir[i] = malloc(100 * sizeof(char));
		memset(filesInDir[i], 0, sizeof(filesInDir[i]));
	}

	return filesInDir;
}

// try to clean up as much as we can
void deleteContainer_filesInDir(char **filesInDir, int size) {
	int i;
	for (i = 0; i < size; i++) {
		free(filesInDir[i]);
	}

	free(filesInDir);
}

// check if the file the client wants is there
int checkForChosenFile(char *filename, char **files, int numFiles) {
	int i;
	for (i = 0; i < numFiles; i++) {
		if (strcmp(files[i], filename) == 0) {
			return 1;
		}
	}

	return 0;
}

// send the files contents back to the client
int sendFile(char *addr, char *port, char *filename) {
//int sendFile(int sockfd, char *filename) {
	sleep(2);
	// set up connection
	struct addrinfo *connection = createConnection(addr, port);
	int sockfd = createSocket(connection);
	estConnection(sockfd, connection);

	// create a buffer to read in to
	char buffer[2000];

	// open file as read only
	int file = open(filename, O_RDONLY);
	while (1) {
		// sanitize buffer
		memset(buffer, 0, sizeof(buffer));

		int bytes = read(file, buffer, sizeof(buffer) - 1);
		
		if (bytes == 0) {
			break;
		}

		if (bytes < 0) {
			error(0, "Failed to read file.\n");
			return 0;
		}

		// dummy variable to make send work, fill it with the buffer
		void* output = buffer;
		while (bytes > 0) {
			// read out of the dummy variable
			int numBytesWritten = send(sockfd, output, sizeof(buffer), 0);
			if (numBytesWritten < 0) {
				error(0, "Failed to send data.\n");
				return 0;
			}
			bytes -= numBytesWritten;
			output += numBytesWritten;
		}
	}

	// sanitize buffer out here, to send end of transmission flag
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "end_of_file");
	send(sockfd, buffer, sizeof(buffer), 0);

	close(sockfd);
	freeaddrinfo(connection);
}

// send the list of files back to the client
void sendDirectory(char *addr, char *port, char **directory, int numOfFiles) { 
//void sendDirectory(int sockfd, char **directory, int numOfFiles) {
	sleep(2);
	// setup connection
	struct addrinfo *connection = createConnection(addr, port);
	int sockfd = createSocket(connection);
	estConnection(sockfd, connection);

	int i;
	for (i = 0; i < numOfFiles; i++) {
		// send name of each file
		// probably safe to assume file names wont be > 100
		send(sockfd, directory[i], 100, 0);
	}

	char* completed = "end_of_list";
	send(sockfd, completed, strlen(completed), 0);

	close(sockfd);
	freeaddrinfo(connection);
}

////int processCmd(int clientfd, char *cmd) {
//int processCmd(int clientfd, int datafd, char *cmd) {
//	if (strcmp(cmd, "g") == 0) {
//		//send(clientfd, good, strlen(good), 0);
//
//		char filename[100];
//		memset(filename, 0, sizeof(filename));
//		recv(clientfd, filename, sizeof(filename) - 1, 0);
//		printf("File: %s requested \n", filename);
//
//		char** files = initContainer_filesInDir(500);
//		int numFiles = getDirectory(files);         //Use the function to check if the file is there
//		int findFile = checkForChosenFile(filename, files, numFiles);
//		if (findFile) {
//
//			printf("File found, sending %s to client\n", filename);
//			char *file_found = "File found";
//			send(clientfd, file_found, strlen(file_found), 0);
//
//			char new_filename[100];
//			memset(new_filename, 0, sizeof(new_filename));
//			strcpy(new_filename, "./");
//			char * end = new_filename + strlen(new_filename);
//			end += sprintf(end, "%s", filename);
//
//			//sendFile(addr, port, new_filename);
//			sendFile(datafd, new_filename);
//		}
//		else {
//			printf("Could not find file. Sending error to client.\n");
//			char * notFound = "File not found";
//			send(clientfd, notFound, 100, 0);
//		}
//		deleteContainer_filesInDir(files, 500);
//	}
//	else if (strcmp(cmd, "l") == 0) {                  //Directory request so get the number of files and send them
//
//		//send(clientfd, good, strlen(good), 0);
//		printf("File list requested \n");
//		printf("Sending file list to Client.\n");
//
//		char** files = initContainer_filesInDir(500);
//
//		int numFiles = getDirectory(files);
//
//		//sendDirectory(addr, port, files, numFiles);
//		sendDirectory(clientfd, files, numFiles);
//
//		deleteContainer_filesInDir(files, 500);
//	}
//	else {
//		//send(clientfd, bad, strlen(bad), 0);
//		printf("Got invalid command.\n");
//	}
//
//}

// good/bad taken from another project I saw while 
void talkToClient(int clientfd) {
	// used to tell the client if their request was valid or not
	char *good = "valid";
	char *bad = "invalid";
	// buffers for holding incoming information
	char addr[100]; // makes connections easier to visualize
	char port[100]; // port client wants to recieve data back on
	char cmd[100]; // command from client

	// sanitize port
	memset(port, 0, sizeof(port));              
	recv(clientfd, port, sizeof(port) - 1, 0);

	send(clientfd, good, strlen(good), 0);

	// sanitize command
	memset(cmd, 0, sizeof(cmd));
	recv(clientfd, cmd, sizeof(cmd) - 1, 0);

	send(clientfd, good, strlen(good), 0);

	// sanitize address
	memset(addr, 0, sizeof(addr));
	recv(clientfd, addr, sizeof(addr) - 1, 0);

	printf("A Client is connecting from: %s\n", addr);

	// send a good/bad response message depending on command,
	// if the command was valid, execute it
	// get a file
	if (strcmp(cmd, "g") == 0) {
		send(clientfd, good, strlen(good), 0);

		char filename[100];
		memset(filename, 0, sizeof(filename));
		recv(clientfd, filename, sizeof(filename) - 1, 0);
		printf("Client requested file: %s\n", filename);

		// get the list of files in the directory
		char **files = initContainer_filesInDir(500);
		int numFiles = getNumFilesInDir(files);
		int fileFound = checkForChosenFile(filename, files, numFiles);
		if (fileFound) {
			printf("File found. Sending \"%s\" to Client.\n", filename);
			char *file_found = "File found";
			send(clientfd, file_found, strlen(file_found), 0);

			// declare file name and clean it
			char new_filename[100];
			memset(new_filename, 0, sizeof(new_filename));
			
			// append current directory extension to beginning of file name
			strcpy(new_filename, "./");
			char *filepathName = new_filename + strlen(new_filename);
			filepathName += sprintf(filepathName, "%s", filename);

			sendFile(addr, port, new_filename);
		}
		else {
			printf("Unknown file: %s.\n", filename);
			char *not_found = "File not found";
			send(clientfd, not_found, strlen(not_found), 0);
		}
		deleteContainer_filesInDir(files, 500);
	}
	// get the list of files
	else if (strcmp(cmd, "l") == 0) {
		send(clientfd, good, strlen(good), 0);
		printf("Sending file list to: %s\n", addr);

		char **files = initContainer_filesInDir(500);

		int numFiles = getNumFilesInDir(files);

		sendDirectory(addr, port, files, numFiles);

		deleteContainer_filesInDir(files, 500);
	}
	// invalid command
	else {
		send(clientfd, bad, strlen(bad), 0);
		printf("Invalid command.\n");
	}

	//// handle command
	//if (strcmp(cmd, "g") == 0 || strcmp(cmd, "l") == 0) {
	//	printf("cmd: %s\n", cmd);
	//	send(clientfd, good, strlen(good), 0);
	//	// create data socket
	//	sleep(2);
	//	//strcpy(cmd, "l");
	//	// set up connection
	//	struct addrinfo *dataConn = createConnection(addr, port);
	//	printf("addr: %s\nport: %s\n", addr, port);
	//	printf("1: %s\n", cmd);
	//	int datafd = createSocket(dataConn);
	//	printf("2: %s\n", cmd);
	//	estConnection(datafd, dataConn);
	//	printf("3: %s\n", cmd);

	//	processCmd(clientfd, datafd, cmd);
	//	//processCmd(datafd, cmd);

	//	printf("4: %s\n", cmd);

	//	close(datafd);
	//	freeaddrinfo(dataConn);
	//}
	//else {
	//	send(clientfd, bad, strlen(bad), 0);
	//}

	printf("Waiting for more connections.\n");
}

// handles incomming connections during idle time
void waitForConnection(int sockfd) {
	struct sockaddr_storage clientAddr;
	socklen_t addrSize;
	int newConnection;

	while (1) {
		addrSize = sizeof(clientAddr);
		newConnection = accept(sockfd, (struct sockaddr *)&clientAddr, &addrSize);
		
		if (newConnection == -1) {
			continue;
		}

		talkToClient(newConnection);
		close(newConnection);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		error(1, "Incorrect Arguments. Try \"./ftserver [port]\"\n");
	}

	printf("Server is now listening on port: %s\n", argv[1]);
	struct addrinfo* res = openConnection(argv[1]);
	int sockfd = createSocket(res);
	bindSocket(sockfd, res);
	listenSocket(sockfd);
	waitForConnection(sockfd);
	freeaddrinfo(res);
}