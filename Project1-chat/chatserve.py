#!/bin/python

# Tucker Lavell
# CS372 - Winter 2019
# Project 1 - chatserve.py
# Help received from friend Dan Drapp

import signal
import sys
from socket import *


_conn_ = 0


# get the servers name
def get_name():
    name = ""

    while len(name) > 10 or len(name) == 0:
        name = raw_input("Enter a name for the Server (10 characters max): ")

    print("name: {}".format(name))
    return name


# save name of server and client
def save_state(connection, server):
    client = connection.recv(1024)
    connection.send(server)
    return client


# connection socket, client name, server name
def chat(sockfd, cli, serv):
    while 1:
        # read in opened socket
        sock_in = sockfd.recv(501)[0:-1]
        if sock_in == "":
            print("Connection Closed by {}".format(cli))
            break

        # print prompt
        print("{}> {}".format(cli, sock_in))

        sock_out = ""
        while len(sock_out) > 500 or len(sock_out) == 0:
            sock_out = raw_input("{}> ".format(serv))

        if sock_out == "\quit":
            print("Closing Connection with " + cli)
            break

        sockfd.send(sock_out)


def signal_handler(sig, frame):
    print("Force Closing Server")
    _conn_.close()
    sys.exit(0)


# main
if __name__ == "__main__":
    # command line args
    if len(sys.argv) != 2:
        print("Incorrect Arguments. Try python chatserve.py [port]")
        exit(1)

    # register sigint to close
    signal.signal(signal.SIGINT, signal_handler)

    server_name = get_name()

    port = sys.argv[1]
    server_socket = socket(AF_INET, SOCK_STREAM)
    server_socket.bind(('', int(port)))

    # start listening to socket
    server_socket.listen(0)

    while 1:
        print("{} is ready for connections on port {}".format(server_name, port))
        # return the connection and address as a tuple
        _conn_, addr = server_socket.accept()
        # save socket info
        client_name = save_state(_conn_, server_name)
        print("{} has connected from {}".format(client_name, addr))

        chat(_conn_, client_name, server_name)

        _conn_.close()

