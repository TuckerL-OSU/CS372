//// Tucker Lavell
//// CS372 - Winter 2019
//// Project 1 - chatclient.c
//// much of the code was taken from beej's code samples and suggestions
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <netdb.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//
//void error(int exitCode, const char *msg) {
//	// use stderr to print error so we can set exit codes
//	// for debugging and potential future use
//	fprintf(stderr, "%s\n", msg);
//
//	exit(exitCode);
//}
//
//struct addrinfo *createConnection(char *addr, char *port) {
//	struct addrinfo hints;
//	struct addrinfo *result;
//	int status = 0;
//
//	memset(&hints, 0, sizeof hints);
//	// version
//	hints.ai_family = AF_INET;
//	// TCP stream
//	hints.ai_socktype = SOCK_STREAM;
//
//	if ((status = getaddrinfo(addr, port, &hints, &result)) != 0) {
//		error(2, "Error getting address.\n");
//	}
//
//	return result;
//}
//
//int createSocket(struct addrinfo *conn) {
//	int sockfd;
//
//	if ((sockfd = socket(conn->ai_family, conn->ai_socktype, conn->ai_protocol)) == -1) {
//		error(-1, "Failed to create socket.\n");
//	}
//
//	return sockfd;
//}
//
//int estConnection(int sockfd, struct addrinfo *conn) {
//	int status = 0;
//
//	if ((status = connect(sockfd, conn->ai_addr, conn->ai_addrlen)) == -1) {
//		error(3, "Failed to establish connection to Server.\n");
//	}
//
//	return status;
//}
//
//int connectToServer(int numArgs, char **args) {
//// create new addrinfo stuct to store ip and port
//	struct addrinfo *connection = createConnection(args[1], args[2]);
//
//	// create the socket with the addrinfo struct
//	int sockfd = createSocket(connection);
//
//	// establish the connection
//	if (estConnection(sockfd, connection) != -1) {
//		// successfully created connection, return socket
//		return sockfd;
//	}
//
//	return -2;
//}
//
//int chat(int sockfd, char *clientName, char *serverName) {
//	// buffers to hold input/output
//	char input[500];
//	char output[500];
//
//	// clear stdin
//	fgets(input, sizeof(input), stdin);
//
//	while (1) {
//		// sanitize the buffers
//		memset(input, 0, sizeof(input));
//		memset(output, 0, sizeof(output));
//
//		// prompt user for input and take in it
//		printf("%s> ", clientName);
//		fgets(input, sizeof(input), stdin);
//
//		// break the loop when user inputs \quit
//		if (strcmp(input, "\\quit\n") == 0) {
//			break;
//		}
//
//		// number of bytes sent
//		int bytes = send(sockfd, input, strlen(input), 0);
//		if (bytes == -1) {
//			error(4, "Data not sent correctly.\n");
//		}
//
//		// check for errors upon recieving response
//		int status = recv(sockfd, output, 500, 0);
//		if (status == -1) {
//			error(5, "Data not recieved correctly.\n");
//		}
//		else if (status == 0) {
//			printf("Connection Terminated by Server.\n");
//			break;
//		}
//		else {
//			printf("%s> %s\n", serverName, output);
//		}
//	}
//
//	close(sockfd);
//	printf("Connection Closed.\n");
//}
//
//// save the state of sockfd to preserve client and server names
//void saveState(int sockfd, char *clientName, char *serverName) {
//	int client = send(sockfd, clientName, strlen(clientName), 0);
//	int server = recv(sockfd, serverName, 10, 0);
//}
//
//int main(int argc, char *argv[]) {
//	char clientName[10];
//	char serverName[10];
//
//	// we need 3 arguments to start the client. [command] [server] [port]
//	if (argc != 3) {
//		error(1, "Incorrect Arguments. Try \"./chatclient [server] [port-on-server]\"\n");
//	}
//	else {
//		printf("Enter a Username (10 characters max): ");
//		// no input validation here please don't try to break it
//		scanf("%s", clientName);
//	}
//
//	int sockfd = connectToServer(argc, argv);
//	if (sockfd == -2) {
//		error(sockfd, "Error in main\n");
//	}
//	else {
//		saveState(sockfd, clientName, serverName);
//		chat(sockfd, clientName, serverName);
//	}
//
//	return 0;
//}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_BUFFER_SIZE 1024

//Print the error message to standard error and exit 1
void error(const char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

// Write the message to socket, output error info if applicable.
int Write(int sockfd, char* msg) {
	//printf("%s\n", msg);
	if (write(sockfd, msg, strlen(msg)) < 0) {
		printf("ERROR writing to socket\n");
		close(sockfd);
		return -1;
	}
	return 1;
}


//Read the message from socket, output error info if applicable
int Read(int sockfd, char* msg, int len) {
	bzero(msg, MAX_BUFFER_SIZE);
	if (read(sockfd, msg, len) < 0) {
		printf("ERROR reading from socket\n");
		close(sockfd);
		return -1;
	}
	return strlen(msg);
}

//Connects to otp_enc_d, when plain text, and return ciphertext.
int main(int argc, char *argv[])
{
	int sockfd, port, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char handle[5] = "Bob> ";
	char buffer[MAX_BUFFER_SIZE];  // buffer for socket read
	char line[MAX_BUFFER_SIZE];  // line for console input
	char socket_line[MAX_BUFFER_SIZE];  // line for socket write
	fd_set read_fds, all_fds;  // file descriptor list for select()
	int fdmax;        // maximum file descriptor number

	//Handle error of the command line
	if (argc < 3) {
		error("Usage: chatclient hostname port");
	}

	//Create the socket endpoint
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		error("ERROR opening socket");
	}

	//Get host and port, setup serv_addr.
	server = gethostbyname(argv[1]);
	if (server == NULL) error("no such host");
	port = atoi(argv[2]);

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr,
		  (char *)&serv_addr.sin_addr.s_addr,
		  server->h_length);
	serv_addr.sin_port = htons(port);

	printf("%s connecting as %d ...\n", handle, sockfd);
	//Connect the socket to the server
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		error("Could not connect to server");
	}
	printf("connected\n%s", handle);
	fflush(stdout);

	// Add stdin and sockfd to the select list
	FD_ZERO(&all_fds);
	FD_SET(0, &all_fds);
	FD_SET(sockfd, &all_fds);
	fdmax = sockfd;
	while (1) {
		read_fds = all_fds;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) > 0) {
			if (FD_ISSET(0, &read_fds)) {
				// There is keyboard input.
				if (fgets(line, sizeof(line), stdin) != NULL) {
					if (strncmp(line, "\\quit", 5) == 0) {
						printf("quit\n");
						break;
					}
					strcpy(socket_line, handle);
					strcat(socket_line, line);
					if (write(sockfd, socket_line, strlen(socket_line)) < 0) {
						printf("ERROR writing to socket\n");
						break;
					}
				}
				printf("%s", handle);
				fflush(stdout);
			}
			if (FD_ISSET(sockfd, &read_fds)) {
				// There is new message from the socket.
				if ((n = read(sockfd, buffer, sizeof(buffer)-1)) > 0) {
					buffer[n] = 0;
					if (buffer[n-1] == '\n') buffer[n-1] = 0;

					printf("\n%s\n%s", buffer, handle);
					fflush(stdout);
				}
			}
		}
	}

	close(sockfd);
	return 0;
}