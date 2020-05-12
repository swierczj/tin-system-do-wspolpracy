package client;

import client.Protocol.Header;
import client.Protocol.Key;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;

import static client.Protocol.Header.MsgType.STATEMENT;
import static client.Protocol.Key.KeyType.PUBLIC;
import static client.Protocol.Statement.Info.PUBLIC_KEY_REQUEST;

public class Client{
    private static String ip = null;
    private static int port = 0;
    private Socket socket;
    private BufferedReader input;
    private PrintWriter output;
    private Scanner scanner;
    private static Thread reader, writer, keepAlive;
    private static volatile boolean isAlive = false;
    private static volatile boolean isRunning = false;
    private static String messageIn = "";
    private static String serverPublicKey = "";

    public Client( String ip, int port ){
        this.ip = ip;
        this.port = port;
    }

    public int getPort(){
        return port;
    }

    public String getIp(){
        return ip;
    }

    public static Thread getKeepAlive(){
        return keepAlive;
    }

    public static Thread getReader(){
        return reader;
    }

    public static Thread getWriter(){
        return writer;
    }

    private void getHeader(){
        Header header = Header.parseFrom( input );
        switch( header.getMsgType() ){
            case PUBLIC_KEY:
                getServerPublicKey();
                break;
        }
    }

    private void askForPublicKey(){
        String msg = "Dawaj mi kurwa ten klucz!";
        Header.Builder header = Header.newBuilder();
        header.setMsgLength( msg.length() );
        header.setMsgType( STATEMENT );
        Protocol.Statement.Builder statement = Protocol.Statement.newBuilder();
        statement.setInfo( PUBLIC_KEY_REQUEST );
        header.build().writeTo( output );
        statement.build().writeTo( output );
    }

    private void getServerPublicKey(){
        Key publicKey = Key.parseFrom( input );
        if( publicKey.getKeyType() == PUBLIC ){
            serverPublicKey = publicKey.getKey();
            System.out.print( "Key received from server\n" );
        }else{
            askForPublicKey();
            System.out.print( "Key not received. I will try again\n" );
        }
    }

    public int connect() throws IOException{
        try{
            socket = new Socket( ip, port );
        }catch( UnknownHostException e ){
            System.out.println( "Cannot connect to " + ip + " on port " + port );
            return -1;
        }catch( IOException e ){
            System.out.println( "IO Exception" );
            return -2;
        }
        isAlive = true;
        isRunning = true;

        input = new BufferedReader( new InputStreamReader( socket.getInputStream() ) );
        output = new PrintWriter( socket.getOutputStream(), true );
        scanner = new Scanner( System.in );

        return 0;
    }

    public int createThreads(){
        createWriter();
        createReader();
        createKeepAlive();
        return 0;
    }

    public void startThreads(){
        writer.start();
        reader.start();
        keepAlive.start();
    }

    public int login(){
        return 0;
    }


    private int createReader(){
        AtomicInteger toReturn = new AtomicInteger( 0 );
        reader = new Thread( () -> {
            try{
                while( isRunning ){
                    messageIn = input.readLine();
                    if( messageIn == null ) break;
                    isAlive = true;
                    System.out.print( "Server: " + messageIn + "\n" );
                    messageIn = "";
                }
            }catch( IOException ex ){
                System.out.print( "reader closed\n" );
                toReturn.set( -1 );
            }
            scanner.close();
            System.out.print( "Server disconnected you\n" );
            isRunning = false;
        } );
        return toReturn.intValue();
    }

    private int createWriter(){
        AtomicInteger toReturn = new AtomicInteger( 0 );
        writer = new Thread( () -> {
            try{
                String messageOut = "connected";
                while( !messageOut.equals( "q" ) && !messageOut.equals( "quit" ) && isRunning ){
                    output.println( messageOut );
                    messageOut = scanner.nextLine();
                }
                socket.close();
                input.close();
                isRunning = false;
            }catch( IOException ex ){
                toReturn.set( -1 );
            }
        } );
        return toReturn.intValue();
    }


    private int createKeepAlive(){
        AtomicInteger toReturn = new AtomicInteger( 0 );
        keepAlive = new Thread( () -> {
            try{
                while( isAlive && isRunning ){
                    Thread.sleep( 10000 );
                    isAlive = false;
                    output.println( "Am I connected?" );
                    Thread.sleep( 5000 );
                }
                System.out.print( "Connection broken" );
                isRunning = false;
                scanner.close();
            }catch( InterruptedException iEx ){
                toReturn.set( -1 );
            }
        } );
        return toReturn.intValue();
    }
}
