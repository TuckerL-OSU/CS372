
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

	// if the exitCode is -1, dont exit
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
		error(3, "Failed to establish connection to server.\n");
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

// this is potential problems
int chat(int sockfd, char *clientName) {
	int bytes = 0;
	int status;
	// buffers to hold input/output
	char input[500];
	char output[500];

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
		bytes = send(sockfd, input, strlen(input), 0);
		if (bytes == -1) {
			error(4, "Data not sent correctly.\n");
		}

		// check for errors upon recieving response
		status = recv(sockfd, output, 500, 0);
		if (status == -1) {
			error(5, "Data no recieved correctly.\n");
		}
		else if (status == 0) {
			printf("Connection terminated by server.\n");
		}
		else {
			printf("%s\n", output);
		}
	}

	close(sockfd);
	printf("Connection Terminated.\n");
}

int main(int argc, char *argv[]) {
	char clientName[10];

	// we need 3 arguments to start the client. [command] [server] [port]
	if (argc != 3) {
		error(1, "Incorrect. Try \"./chatclient [server] [port-on-server]\"\n");
	}
	else {
		printf("Enter a username (up to 10 characters): ");
		// no input validation here please don't try to break it
		scanf("%s", clientName);
	}

	int sockfd = connectToServer(argc, argv);
	if (sockfd == -2) {
		error(sockfd, "Error in main\n");
	}
	else {
		chat(sockfd, clientName);
	}

	return 0;
}