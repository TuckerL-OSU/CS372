import java.net.*;
import java.io.*;

public class chatserve {
    public static String serverName = ""; //The Server's screen name
    public static String clientName = ""; //The client's screen name

    public static ServerSocket initServer(int port) {
//      public static Socket initServer(int port) {
        try (  //try-with-resources: set up socket, wait for client, set up streams.
               ServerSocket serverSocket = new ServerSocket(port);
        ) {
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
