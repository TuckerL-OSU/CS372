import java.net.*;
import java.io.*;
import java.util.Scanner;

public class chatserve {
    public static void main(String[] args) throws IOException {

        if(args.length != 1) {
            System.err.println("Enter port number on command line");
            System.exit(1);
        }

        int serverPort = Integer.parseInt(args[0]);

        System.out.println("Enter the Servers name (10 characters max): ");
        Scanner sc = new Scanner(System.in);
        String serverName = sc.nextline();
        ServerSocket serverSocket = new ServerSocket(serverPort);
        System.out.println(serverName + " is waiting for a connection on port " + serverPort);

        // repeatedly wait for connections, and process
        while (true) {
            // a "blocking" call which waits until a connection is requested
            Socket clientSocket = serverSocket.accept();
            System.out.println("Accepted connection from client");

            // open up IO streams
            BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);

            String sentence = " ";
            String clientResp;
            String quit = "\\quit";
            Scanner hosta = new Scanner(System.in); //input from host A user

            while ((clientResp = in.readLine()) != null) {
                //client calls \quit
                if(clientResp.equals(quit)) {
                    break;
                }
                System.out.println(clientResp);
                System.out.print(serverName);
                sentence = hosta.nextLine();
                //server calls \quit
                if(sentence.equals(quit)) {
                    out.println(sentence);
                    break;
                }
                out.println(serverName + "> " + sentence);
            }

            // close IO streams, then socket
            System.out.println("Closing connection with client");
            out.close();
            in.close();
            clientSocket.close();
        }
    }
}