// Tucker Lavell
// CS372 - Winter 2019
// Project 1 - chatclient.c
// much of the code was taken from beej's code samples and suggestions
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

void error(int exitCode, const char *msg) {
	// use stderr to print error so we can set exit codes
	// for debugging and potential future use
	fprintf(stderr, "%s\n", msg);

	exit(exitCode);
}

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

int estConnection(int sockfd, struct addrinfo *conn) {
	int status = 0;

	if ((status = connect(sockfd, conn->ai_addr, conn->ai_addrlen)) == -1) {
		error(3, "Failed to establish connection to Server.\n");
	}

	return status;
}

int connectToServer(int numArgs, char **args) {
// create new addrinfo stuct to store ip and port
	struct addrinfo *connection = createConnection(args[1], args[2]);

	// create the socket with the addrinfo struct
	int sockfd = createSocket(connection);

	// establish the connection
	if (estConnection(sockfd, connection) != -1) {
		// successfully created connection, return socket
		return sockfd;
	}

	return -2;
}

int chat(int sockfd, char *clientName, char *serverName) {
	// buffers to hold input/output
	char input[500];
	char output[500];

	// clear stdin
	fgets(input, sizeof(input), stdin);

	while (1) {
		// sanitize the buffers
		memset(input, 0, sizeof(input));
		memset(output, 0, sizeof(output));

		// prompt user for input and take in it
		printf("%s> ", clientName);
		fgets(input, sizeof(input), stdin);

		// break the loop when user inputs \quit
		if (strcmp(input, "\\quit\n") == 0) {
			break;
		}

		// number of bytes sent
		int bytes = send(sockfd, input, strlen(input), 0);
		if (bytes == -1) {
			error(4, "Data not sent correctly.\n");
		}

		// check for errors upon recieving response
		int status = recv(sockfd, output, 500, 0);
		if (status == -1) {
			error(5, "Data not recieved correctly.\n");
		}
		else if (status == 0) {
			printf("Connection Terminated by Server.\n");
			break;
		}
		else {
			printf("%s> %s\n", serverName, output);
		}
	}

	close(sockfd);
	printf("Connection Closed.\n");
}

// save the state of sockfd to preserve client and server names
void saveState(int sockfd, char *clientName, char *serverName) {
	int client = send(sockfd, clientName + '\0', strlen(clientName) + 1, 0);
	int server = recv(sockfd, serverName, 10, 0);
	printf("we got back: %s\n", serverName);
}

int main(int argc, char *argv[]) {
	char clientName[10];
	char serverName[10];

	// we need 3 arguments to start the client. [command] [server] [port]
	if (argc != 3) {
		error(1, "Incorrect Arguments. Try \"./chatclient [server] [port-on-server]\"\n");
	}
	else {
		printf("Enter a Username (10 characters max): ");
		// no input validation here please don't try to break it
		scanf("%s", clientName);
	}

	int sockfd = connectToServer(argc, argv);
	if (sockfd == -2) {
		error(sockfd, "Error in main\n");
	}
	else {
		saveState(sockfd, clientName, serverName);
		chat(sockfd, clientName, serverName);
	}

	return 0;
}

