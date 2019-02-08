import java.net.*;
import java.io.*;

//public class chatserve {
//    public static String serverName = ""; //The Server's screen name
//    public static String clientName = ""; //The client's screen name
//
//    public static boolean processInput(Socket clientSocket, BufferedReader in) {
//        /**
//         * Process tcp data from client
//         * @param clientSocket The socket descriptor for the current client
//         * @param in The byte stream to read from
//         * @return false on error or quit, true otherwise
//         */
//        String inputLine = "";
//        try {  //Read from the socket.
//            inputLine = in.readLine();
//        }
//        catch(IOException ie) {
//            System.out.println("Conversation with client is disconnected");
//            return false;  //Return to main to end the program.
//        }
//
//        clientName = inputLine;
//
//        if (inputLine == null || inputLine.length() < 15) {
//            System.out.println(clientName + " has disconnected");
//            return false;
//        }
//        // clientName = inputLine.substring(0, 10).replace("_", "");
//        // String msg = inputLine.substring(10, inputLine.indexOf("<END>"));
//        // System.out.println(clientName + ">" + msg);
//        // System.out.print(serverName.replace("_", "") + "> ");
//
//        String msg = inputLine.substring(clientName.length(), inputLine.indexOf("\0"));
//        System.out.println(clientName + "> " + msg);
//        System.out.print(serverName + "> ");
//        return true;
//    }
//
//    public static boolean processOutput(String serverName, BufferedReader consoleInput, PrintWriter out) {
//        /**
//         * Process and send command line input from Server user.
//         * @param serverName The server's screen name
//         * @param consoleInput The terminal input byte stream
//         * @param out The byte stream to write to
//         * @return false on error or quit, true otherwise.
//         */
//        String input = "";
//        try { //Read from terminal
//            input = consoleInput.readLine();
//        } catch (Exception e){
//            System.err.println("Invalid input");
//            return false;  //Return to main to end program.
//        }
//
//        if (input.contains("\\quit")) {  //server user typed '\quit'
//            System.out.println("You disconnected from " + clientName);
//            return false;  //Return to main to end program.
//        }
//        else {  //Prepend username, append end signal to user input, and send.
//            // out.println(serverName + input + "<END>");
//            out.println(serverName + input);
//            return true;
//        }
//    }
//
//    public static void closeClient(Socket clientSocket) {
//        /**
//         * Gracefully close client socket
//         * @param clientSocket The current open tcp connection
//         * @return exit gracefully on error.
//         */
//        try {  //close tcp connection to client.
//            clientSocket.close();
//        }
//        catch (Exception e) {
//            System.out.println("There was an error disconnecting from " + clientName);
//            System.exit(1);
//        }
//    }
//
//    public static void main(String[] args) {
//        /**
//         * serverName cli, Establish server socket, wait for connections, manage connections
//         * @param args User-inputted port number
//         */
//
//        // Usage statement in case of incorrect args input.
//        String Usage = "Usage: $java Chatserve <Port Number>\n";
//        int port = 0; //port number (input by user)
//
//        if (args.length > 1) {  //Too many arguments
//            System.err.println("Only 1 argument permitted\n" + Usage);
//            return;  //Close program.
//        }
//        else if(args.length == 0) {  //Missing required port argument.
//            System.err.println("Port argument required\n" + Usage);
//            return;  //Close program.
//        }
//        /* CLI parsing help obtained from
//         * docs.oracle.com/javase/tutorial/essential/environment/cmdLineArgs.html*/
//        else {
//            try {   //Verify user inputted port number.
//                port = Integer.parseInt(args[0]);
//            } catch (NumberFormatException e) {
//                System.err.println("Port argument must be an integer.\n" + Usage);
//                return;  //Close program.
//            }
//        }
//
//        // Valid port number entered. Get user to input username.
//
//        System.out.print("Welcome to chatserver on port " +
//                Integer.toString(port) +
//                "\nPlease enter a serverName: ");
//
//        /* Help with terminal input athspk's answer on the following page:
//         * http://stackoverflow.com/questions/4644415/java-how-to-get-input-from-system-console
//         */
//
//        // User input stream.
//        BufferedReader consoleInput =  new BufferedReader(new InputStreamReader(System.in));
//        try {  //Read user input from terminal.
//            serverName = consoleInput.readLine();
//            if (serverName.length() > 10) {  //serverName is too long: truncate.
//                serverName = serverName.substring(0, 10);
//                System.out.println("serverName truncated to: " + serverName);
//                // } else {  //Pad inputted serverName with underscores (removed on print)
//                // int spaces = 10 - serverName.length();
//                // for (int i = 0; i < spaces; i++) {
//                // serverName += '_';
//                // }
//
//                // Print accepted username
//                // System.out.println("serverName: " + serverName.replace("_", ""));
//                System.out.println("serverName: " + serverName);
//            }
//        } catch(Exception e) {  //Invalid serverName.
//            System.err.println("\nInvalid serverName input. ");
//            return;  //close program
//        }
//
//        /**
//         * The main loop of Chatserve.
//         * Each iteration is a different client connection.
//         */
//        while (true) {
//
//            System.out.println("Waiting for a connection...");
//
//            /* Code help obtained from docs.oracle.com/javase/tutorial/networking/sockets/clientServer.html */
//            try (  //try-with-resources: set up socket, wait for client, set up streams.
//                   ServerSocket serverSocket = new ServerSocket(port);
//                   Socket clientSocket = serverSocket.accept();
//                   PrintWriter out =
//                           new PrintWriter(clientSocket.getOutputStream(), true);
//                   BufferedReader in = new BufferedReader(
//                           new InputStreamReader(clientSocket.getInputStream()));
//            ) {
//                System.out.println("A client has connected. Awaiting initial message. . .");
//                out.println(serverName);
//
//                /**
//                 * The connection loop.
//                 * Each iteration is 1 message exchange
//                 */
//                while (true) {
//                    // Print message from client.
//                    if (!processInput(clientSocket, in)) {
//                        closeClient(clientSocket);
//                        break;
//                    }
//
//                    // Send message from server user.
//                    if (!processOutput(serverName, consoleInput, out)) {
//                        closeClient(clientSocket);
//                        break;
//                    }
//                }
//
//            } catch (Exception ie) {
//                return;  //close program on error.
//            }  //Client loop
//        }  //Main loop
//    }  //void main()
//}  //class Chatserve

public class chatserve {
    public static String serverName = ""; //The Server's screen name
    public static String clientName = ""; //The client's screen name

    public static ServerSocket initServer(int port) {
//      public static Socket initServer(int port) {
        try (  //try-with-resources: set up socket, wait for client, set up streams.
               ServerSocket serverSocket = new ServerSocket(port);
        ) {
            // set a timeout so it will wait
            serverSocket.setSoTimeout(10000);
            System.out.println(serverName + " started on port: " + port + ".\n");
            return serverSocket;
        } catch (IOException ie) {
            System.out.println("Failed to start " + serverName + " on port: " + port + ".\n");
            System.exit(1);  //close program on error.
            return null;
        }
    }

    // pass servers socket
    // need a way to send server name
    public static Socket estConnection(ServerSocket server) {
//    public static Socket estConnection(Socket server) {
        try (
                Socket client = server.accept();
                BufferedReader clientSYN = new BufferedReader(new InputStreamReader(client.getInputStream()));
                PrintWriter serverACK = new PrintWriter(client.getOutputStream(), true);
        ) {
            clientName = clientSYN.readLine();
            serverACK.println(serverName);
            System.out.println(clientName + " has successfully connected.\n");
            return client;
        } catch (IOException ie) {
            System.out.println(serverName + " failed to connect to client.\n");
            System.exit(1);
            return null;
        }
    }

    public static void getName() {
        System.out.print("Enter a name for this Server (10 characters max): ");
        BufferedReader consoleInput = new BufferedReader(new InputStreamReader(System.in));
        try {  //Read user input from terminal.
            serverName = consoleInput.readLine();
        } catch (Exception e) {  //Invalid serverName.
            System.err.println("Invalid serverName.\n");
            System.exit(1);
        }
    }

    public static boolean processInput(Socket client) {
               String input = "";
        try {  //Read from the socket.
            BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
            input = in.readLine();
        } catch (IOException ie) {
            System.out.println("Conversation with client is disconnected\n");
            return false;  //Return to main to end the program.
        }
        if (input == null) {
            System.out.println(clientName + " has disconnected\n");
            return false;
        }
        String msg = input.substring(clientName.length(), input.indexOf("\0"));
        System.out.println(clientName + "> " + msg);
        System.out.print(serverName + "> ");
        return true;
    }

    public static boolean processOutput(Socket client) {
        String input = "";
        try (
            BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
            PrintWriter out = new PrintWriter(client.getOutputStream(), true);
        ) {
            input = in.readLine();
            if (input.contains("\\quit")) {
                System.out.println("You closed the connection with: " + clientName + ".\n");
                return false;
            } else {
                out.println(serverName + input);
                return true;
            }
        } catch (Exception e) {
            System.err.println("Invalid input\n");
            return false;
        }
    }

    public static void chat(Socket client) {
        while (true) {
            if (!processInput(client)) {
                termConnection(client);
                break;
            }

            if (!processOutput(client)) {
                termConnection(client);
                break;
            }
        }
    }

    public static void termConnection(Socket client) {
        try {
            client.close();
        } catch (Exception e) {
            System.out.println("Error closing connection with " + clientName + ".\n");
            System.exit(1);
        }
    }

    // PrintWriter out = new PrintWriter(client.getOutputStream(), true);

    public static void main(String[] args) {
        // Usage statement in case of incorrect args input.
        ServerSocket server;
//        Socket server;
        Socket client;

        // arguments bad
        if (args.length != 1) {
            System.err.println("Incorrect Arguments. Try: \"java chatserve [port]\"\n");
            return;  //Close program.
        } else {
            getName();
        }

        int port = Integer.parseInt(args[0]);
        server = initServer(port);

        while (true) {
            System.out.println("Waiting for a connection...");
//            client = initServer(port);
            client = estConnection(server);
            chat(client);
        }
    }
}




//    System.out.print("Welcome to chatserver on port " + port +
//                "\nPlease enter a serverName: ");
//
//        /* Help with terminal input athspk's answer on the following page:
//         * http://stackoverflow.com/questions/4644415/java-how-to-get-input-from-system-console
//         */
//
//        /**
//         * The main loop of Chatserve.
//         * Each iteration is a different client connection.
//         */
//        while (true) {
//
//            System.out.println("Waiting for a connection...");
//
//            /* Code help obtained from docs.oracle.com/javase/tutorial/networking/sockets/clientServer.html */
//            try (  //try-with-resources: set up socket, wait for client, set up streams.
//                   ServerSocket serverSocket = new ServerSocket(port);
//                   Socket clientSocket = serverSocket.accept();
//                   PrintWriter out =
//                           new PrintWriter(clientSocket.getOutputStream(), true);
//                   BufferedReader in = new BufferedReader(
//                           new InputStreamReader(clientSocket.getInputStream()));
//            ) {
//                System.out.println("A client has connected. Awaiting initial message. . .");
//                out.println(serverName);
//
//                /**
//                 * The connection loop.
//                 * Each iteration is 1 message exchange
//                 */
//                while (true) {
//                    // Print message from client.
//                    if (!processInput(clientSocket, in)) {
//                        closeClient(clientSocket);
//                        break;
//                    }
//
//                    // Send message from server user.
//                    if (!processOutput(serverName, consoleInput, out)) {
//                        closeClient(clientSocket);
//                        break;
//                    }
//                }
//
//            } catch (Exception ie) {
//                return;  //close program on error.
//            }  //Client loop
//        }  //Main loop
//    }
//}