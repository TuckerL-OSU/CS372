// Tucker Lavell
// CS372 - Winter 2019
// Project 2 - ftserver.c
// Much of the socket code was taken from beej's code samples and suggestions.
// I was also able to reuse some of my code from P1 and from a past class.
// Noted within are extra help I needed/found.
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
		error(3, "Failed to establish connection from Client.\n");
	}

	return status;
}

// directory navigation: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
// ignore not files: https://www.gnu.org/software/libc/manual/html_node/Directory-Entries.html
int getDirectory(char** files) {
	DIR* fd;
	struct dirent *dir;
	fd = opendir(".");
	int i = 0;
	if (fd) {
		while ((dir = readdir(fd)) != NULL) {     //While there are still things to read, read in the file names
			// only get regular files (ignores . and ..)
			if (dir->d_type == DT_REG) {
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

void deleteContainer_filesInDir(char **filesInDir, int size) {     //Function to clear the array
	int i;
	for (i = 0; i < size; i++) {
		free(filesInDir[i]);
	}

	free(filesInDir);
}

// check if the file the client wants is there
int checkForChosenFile(char **files, int numFiles, char *filename) {
	//int fileFound = 0;
	int i;

	//for (i = 0; i < numFiles; i++) {
	//	if (strcmp(files[i], filename) == 0) {
	//		fileFound = 1;
	//	}
	//}
	for (i = 0; i < numFiles; i++) {
		if (strcmp(files[i], filename) == 0) {
			return 1;
		}
	}

	return 0;

	//return fileFound;
}

// send the files contents back to the client
int sendFile(char *addr, char *port, char *filename) {                //This is what we'll use to send the file
	sleep(2);
	struct addrinfo *connection = createConnection(addr, port);      // Call our function to create the address
	int sockfd = createSocket(connection);                                               //Create the socket and then connect it. These are all functions we wrote from Beej's guide
	estConnection(sockfd, connection);
																						 // establish the connection
	//if (estConnection(sockfd, connection) == -1) {
	//	// did not establish connection, return socket
	//	return sockfd;
	//}

	char buffer[2000];                                                                          //Create a buffer for the file and clear it with memset	

	int file = open(filename, O_RDONLY);                                                      //While loop to read the file. Only need to read it
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

		void* output = buffer;
		while (bytes > 0) {
			int numBytesWritten = send(sockfd, output, sizeof(buffer), 0);
			if (numBytesWritten < 0) {
				error(0, "Failed to send data.\n");
				return 0;
			}
			bytes -= numBytesWritten;
			output += numBytesWritten;
		}
	}

	// sanitize buffer out here, to send done
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, "__done__");
	send(sockfd, buffer, sizeof(buffer), 0);

	close(sockfd);
	freeaddrinfo(connection);
}

// send the list of files back to the client
void sendDirectory(char *addr, char *port, char **files, int numOfFiles) {      //Function to send directory
	sleep(2);
	struct addrinfo *connection = createConnection(addr, port);      //Similar setup for connections
	int sockfd = createSocket(connection);
	estConnection(sockfd, connection);

	//if (estConnection(sockfd, connection) == -1) {
	//	// successfully created connection, return socket
	//	return sockfd;
	//}

	int i;
	for (i = 0; i < numOfFiles; i++) {
		// ,,100,0
		send(sockfd, files[i], 100, 0);                 //Send for the total number of files
	}

	char* completed = "done";
	send(sockfd, completed, strlen(completed), 0);

	close(sockfd);
	freeaddrinfo(connection);
}

void talkToClient(int new_fd) {	            //Structure for this borrowed from Beej's guide
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

		char** files = initContainer_filesInDir(500);
		int numFiles = getDirectory(files);         //Use the function to check if the file is there
		int findFile = checkForChosenFile(files, numFiles, filename);
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
		deleteContainer_filesInDir(files, 500);
	}

	else if (strcmp(command, "l") == 0) {                  //Directory request so get the number of files and send them

		send(new_fd, good, strlen(good), 0);
		printf("File list requested \n");
		printf("Sending file list to %s \n", ipAddress);

		char** files = initContainer_filesInDir(500);

		int numFiles = getDirectory(files);

		sendDirectory(ipAddress, port, files, numFiles);

		deleteContainer_filesInDir(files, 500);
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

		talkToClient(new_fd);
		close(new_fd);
	}
}


int main(int argc, char *argv[]) {
	if (argc != 2) {                  //We already know the usage is the executable plus the port number
		fprintf(stderr, "Usage: ./ftserver [server]\n");
		exit(1);
	}

	printf("Server now listening on port %s\n", argv[1]);   //Now we just call all the functions we wrote in main
	struct addrinfo* res = openConnection(argv[1]);
	int sockfd = createSocket(res);
	bindSocket(sockfd, res);
	listenSocket(sockfd);
	waitForConnection(sockfd);
	freeaddrinfo(res);
}