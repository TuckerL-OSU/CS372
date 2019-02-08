import java.net.*;
import java.io.*;
import java.nio.CharBuffer;

public class chatserve {
    public static String serverName = null; //The Server's screen name
    public static String clientName = null; //The client's screen name
    private static class ConnInfo {
//    abstract class ConnInfo {
        Socket conn;
        BufferedReader console;
        BufferedReader input;
        PrintWriter output;
    }

//    public static ServerSocket initServer(int port) {
////      public static Socket initServer(int port) {
//        try (  //try-with-resources: set up socket, wait for client, set up streams.
//               ServerSocket serverSocket = new ServerSocket(port);
//        ) {
//            System.out.println(serverName + " started on port: " + port + ".");
//            return serverSocket;
//        } catch (IOException ie) {
//            System.out.println("Failed to start " + serverName + " on port: " + port + ".");
//            System.exit(1);  //close program on error.
//            return null;
//        }
//    }

//    public static ConnInfo estConnection(ServerSocket server) {
    public static ConnInfo estConnection(int port) {
        try (
                ServerSocket server = new ServerSocket(port);
                Socket client = server.accept();
                BufferedReader fromClient = new BufferedReader(new InputStreamReader(client.getInputStream()));
                PrintWriter toClient = new PrintWriter(client.getOutputStream(), true);
                ){
            System.out.print("SYN\n");
            toClient.print(serverName);
            System.out.print("ACK\n");
//            System.out.println(clientName + " has successfully connected.\n");

            ConnInfo clientConn = new ConnInfo();
            clientConn.conn = client;
            clientConn.input = fromClient;
//            System.out.println("input: " + clientConn.input.readLine());
            clientConn.output = toClient;
            return clientConn;
        } catch (Exception e) {
            System.out.println(serverName + " failed to connect to client.");
//            System.exit(1);
            return null;
        }
    }

    public static void getName() {
        System.out.print("Enter a name for this Server (10 characters max): ");
        BufferedReader consoleInput = new BufferedReader(new InputStreamReader(System.in));
        try {  //Read user input from terminal.
            serverName = consoleInput.readLine();
//            if (serverName.length() < 10) {
//                int padding = 10 - serverName.length();
//                for (int i = 0; i < padding; i++) {
//                    serverName += "_";
//                }
//            } else {
//                serverName = serverName.substring(0, 10);
//                System.out.println("Name truncated to: " + serverName);
//            }
        } catch (Exception e) {  //Invalid serverName.
            System.err.println("Invalid serverName.");
            System.exit(1);
        }
    }

    public static boolean processInput(ConnInfo client) {
//    public static boolean processInput(BufferedReader client) {
        String input = "";
//        String[] temp = null;
//        char[] c = null;
//        StringBuilder sb = new StringBuilder();

        try {  //Read from the socket.
//            BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));

            CharBuffer temp = CharBuffer.allocate((500));
            if (client.input.ready()) {
//                input = client.input.readLine();
                client.input.read(temp);
            }
            input.equals(temp);
            System.out.print("input: " + client.input.readLine().length());


//            while ((temp = client.input.readLine()) != null) {
////            while (!(temp = client.input.readLine()).equals('\n')) {
//                if (temp.length() == 0) {
//                    return false;
//                }
//                else {
//                    input = temp;
//                    clientName = input.substring(0, 10);
//                    String msg = input.substring(clientName.length(), input.indexOf("\0"));
//                    System.out.print(clientName + "> " + msg + "\n");
//                    System.out.print(serverName + "> ");
//                    return true;
//                }
//            }
//            while (client.input.read(c, 0, client.input.readLine().length() - 1) != -1) {
//                int i = 0;
//                temp[i].equals(c);
//
//                i++;
//            }

        } catch (IOException ie) {
            System.out.println("Conversation with client is disconnected");
//            System.out.print("input: " + client.input.readLine().length());
//            System.out.println("input = " + client.input.readLine());
            return false;
        }
//        input.equals(temp);
//        clientName = input.substring(0, 10);
//        if (input == null || input.length() < 15) {
//            System.out.println(clientName + " has disconnected");
//            return false;
//        }
        clientName = input.substring(0, 10);
        String msg = input.substring(clientName.length(), input.indexOf("\0"));
        System.out.print(clientName + "> " + msg + "\n");
        System.out.print(serverName + "> ");
        return true;
//        return false;
    }

    public static boolean processOutput(ConnInfo client) {
        String input;
        try {
//            BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
//            PrintWriter out = new PrintWriter(client.getOutputStream(), true);
            input = client.input.readLine();
            if (input.contains("\\quit")) {
                System.out.println("Connection with " + clientName + " closed.");
                return false;
            } else {
                client.output.print(serverName + input);
                return true;
            }
        } catch (Exception e) {
            System.err.println("Invalid input");
            return false;
        }
    }

    public static void chat(ConnInfo client) {
        System.out.print("before while\n");
        while (true) {
            System.out.println("input: " + client.input);
            if (!processInput(client)) {
                System.out.print("processInput\n");
                System.out.println("clientName: " + clientName);
                termConnection(client.conn);
                break;
            }

            if (!processOutput(client)) {
                System.out.print("processOutput\n");
                termConnection(client.conn);
                break;
            }
        }
    }

    public static void termConnection(Socket client) {
        try {
            client.close();
        } catch (Exception e) {
            System.out.println("Error closing connection with " + clientName + ".");
            System.exit(1);
        }
    }

    // PrintWriter out = new PrintWriter(client.getOutputStream(), true);

    public static void main(String[] args) {
        ServerSocket server;
//        Socket server;
        ConnInfo client ;

        if (args.length > 1) {
            System.err.println("Incorrect Arguments. Try: \"java chatserve [port]\"");
            return;  //Close program.
        } else {
            getName();
        }

        int port = Integer.parseInt(args[0]);
//        server = initServer(port);

        while (true) {
            System.out.println("Waiting for a connection...");
//            client = initServer(port);
//            client = estConnection(server);
            client = estConnection(port);
            chat(client);
        }
    }
}
