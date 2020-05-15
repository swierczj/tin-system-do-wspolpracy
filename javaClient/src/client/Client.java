package client;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;


public class Client{
    private static final int HEADER_STATEMENT_LENGTH = 2;
    private static final int HEADER_LENGTH_LENGTH = 2;
    private static final int LOGIN = 0;
    private static final int STATEMENT = 1;
    private static final int EDIT = 2;
    private static final int PUBLIC_KEY = 3;
    private static final int KEEP_ALIVE = 0;
    private static final int REQUEST_LOGIN = 1;
    private static final int LOGIN_ACCEPTED = 2;
    private static final int LOGIN_REJECTED = 3;
    private static final int LOG_OUT = 4;
    private static final int WORK_END = 5;
    private static final int PUBLIC_KEY_REQUEST = 6;


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

    private String readLine( int len ) throws IOException{
        char[] msg = new char[ 6 ];
        int readChars = input.read( msg, 0, len );
        if( readChars != 6 ) return null;        // Message is null - end of story
        return new String( msg );
    }

    private String intToString( int value, int length ){    // returns string of length "length", fulfill with zeros
        int intLength = String.valueOf( value ).length();
        String header = "";
        if( intLength > length ) return "";
        for( int i = length; i > intLength; --i )
            header += "0";
        header += "value";
        return header;
    }

    private String makeHeader( int type, int length ){
            return intToString( type, HEADER_STATEMENT_LENGTH ) + intToString( length, HEADER_LENGTH_LENGTH );
    }

    private int getHeader() throws IOException{
        int type = -1, msgLength = 0;
        String header = readLine( 6 );
        if( header == null ){
            System.out.print( "Improper header\n" );
            return -1;
        }
        try{
            type = Integer.parseInt( header.substring( 0, 2 ) );
            msgLength = Integer.parseInt( header.substring( 2, 6 ) );
        } catch( NumberFormatException exception ) { System.out.print( "Kurwa cyfry mają byc!!! A nie jakies: " + header "\n"); };
        System.out.print( "Header: type = " + type + ", message length = " + msgLength + '\n' );
        isAlive = true;
        switch( type ){
            case STATEMENT:
                getStatement( msgLength );
                break;
            case EDIT:
                getEdit( msgLength );
                break;
            case PUBLIC_KEY:
                getServerPublicKey( msgLength );
                break;
        }
        return 0;
    }

    private void getEdit( int msgLength ) throws IOException{
        Protocol edit = new Protocol( readLine( msgLength ), EDIT );
        System.out.print( edit.getMessage() );
    }

    private void getStatement( int msgLength ) throws IOException{
        Protocol statement = new Protocol( readLine( msgLength ), STATEMENT );
        switch( statement.getStatement() ){
            case KEEP_ALIVE:
                isAlive = true;
                break;
            case REQUEST_LOGIN:
                login();
                break;
            case LOGIN_ACCEPTED:
                System.out.print( "Login accepted\n" );
                break;
            case LOGIN_REJECTED:
                System.out.print( "Login rejected. Try again\n" );
                login();
                break;
            default:
                System.out.print( "No i co z tego???\n" );
        }
    }

    private void getServerPublicKey( int msgLength ) throws IOException{
        Protocol key = new Protocol( readLine( msgLength ), PUBLIC_KEY );
        if( key.getKeyType() == 1 ){
            serverPublicKey = key.getKey();
            System.out.print( "Key received from server\n" );
        }else{
            writeStatement( PUBLIC_KEY_REQUEST );
            System.out.print( "It's not public key. I will try again\n" );
        }
    }

    private void login(){
        System.out.print( "Login: " );
        String login = scanner.nextLine();
        System.out.print( "Password: " );
        String password = new String( System.console().readPassword() );
        Protocol loginMsg = new Protocol( login, password );
        output.println( makeHeader( LOGIN, loginMsg.getMessage().length() ) );
        output.println( loginMsg.getMessage() );
    }

    private void writeEdit( String msg ){
        Protocol editMsg = new Protocol( msg );
        output.println( makeHeader( EDIT, editMsg.getMessage().length() ) );
        output.println( editMsg.getMessage() );
    }

    private void writeStatement( int statement ){
        Protocol statMsg = new Protocol( statement );
        output.println( makeHeader( STATEMENT, statMsg.getMessage().length() ) );
        output.println( statMsg.getMessage() );
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

    private int createReader(){
        AtomicInteger toReturn = new AtomicInteger( 0 );
        reader = new Thread( () -> {
            while( true ){
                try{
                    if( !( isRunning && getHeader() == 0 ) ) break;
                }catch( IOException ignored ){}
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
                    writeEdit( messageOut );
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
                    writeStatement( KEEP_ALIVE );
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
