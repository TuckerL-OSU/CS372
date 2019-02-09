import sys
from socket import *


# get the servers name
def get_name():
    name = ""

    while 10 < len(name) < 1:
        name = input("Enter a name for the Server (10 characters max): ")

        return name


def save_state(connection, server):
    client = connection.recv(1024)
    connection.send(server)
    return client


# connection socket, client name, server name
def chat(sockfd, cli, serv):
    """Fetch a list of words from a URL.

    Args:
        url: The URL of a UTF-8 text document.

    Returns:
        A list of strings containing the words from the document.
    """
    msg = ""

    while 1:
        # read in opened socket
        sock_in = sockfd.recv(501)[0:-1]
        if sock_in == "":
            print("Nothing received. Closing Connection")
            break

        # print prompt
        print("{}> {}".format(cli, sock_in))

        sock_out = ""
        while 0 <= len(sock_out) < 500:
            sock_out = input("{}> ".format(serv))

        if sock_out == "\\quit":
            print("Closing Connection with " + cli)
            break;

        sockfd.send(sock_out)


# main
if __name__ == "__main__":
    # command line args
    if len(sys.argv) != 2:
        print("Incorrect Arguments. Try python chatserve.py [port]")
        exit(1)

    server_name = get_name()

    port = sys.argv[1]
    server_socket = socket(AF_INET, SOCK_STREAM)
    server_socket.bind(('', int(port)))

    # start listening to socket
    server_socket.listen(0)

    while 1:
        print(server_name + " is ready for connections on port " + port)
        # return the connection and address as a tuple
        conn, addr = server_socket.accept()
        # save socket info
        client_name = save_state(conn, server_name)
        print(client_name + " has connected from {}".format(addr))

        chat(conn, client_name, server_name)

        conn.close()



