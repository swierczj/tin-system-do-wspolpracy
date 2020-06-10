package web.client;

import java.net.*;
import java.io.*;
import java.util.Scanner;

// Michał 185.20.175.81:54000
public class Client {
    private Socket socket = null;
    private BufferedReader input = null;
    private PrintWriter output = null;
    private Thread writer;
    private Thread reader;
    private Boolean isRunning;
    private static String line = "";
    public Client(String address, int port) {
        try {
            socket = new Socket(address, port);
            System.out.println("Connected");

            input = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            output = new PrintWriter(socket.getOutputStream());
            isRunning = true;
        } catch (UnknownHostException u) {
            System.out.println(u);
        } catch (IOException i) {
            System.out.println(i);
        }
        Scanner scanner = new Scanner(System.in);
        writer = new Thread( () -> {
            try{
                String lineOut = "connected";
                while(  !lineOut.equals( "quit" ) && isRunning ){
                    output.println( lineOut );
                    lineOut = scanner.nextLine();
                }
                socket.close();
                input.close();
                isRunning = false;
            }
            catch( IOException ex ){};
        });

        reader = new Thread( () -> {
            try{
                while( isRunning ){
                    line = input.readLine();
                    if( line == null ) break;
                    System.out.print( "Server: " + line + "\n" );
                    line = "";
                }
                scanner.close();
                System.out.print( "Closing connection" );
                isRunning = false;
            }catch( IOException ex ){System.out.print( "OOPS" );};
        });
        reader.start();
        writer.start();
    }

    public static void main(String args[]) {
        Client client = new Client("25.139.176.21", 54000);
    }
}