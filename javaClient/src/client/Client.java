package client;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;


public class Client{
    private static final int HEADER_STATEMENT_LENGTH = 2;
    private static final int HEADER_LENGTH_LENGTH = 4;
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
    private String login;
    private String password;
    private boolean logged = false;
    private boolean canLogIn = false;

    public Client( String ip, int port ){
        Client.ip = ip;
        Client.port = port;
    }

    public void setLoginData( String login, String password ){
        this.login = login;
        this.password = password;
    }

    private String readLine( int len ) throws IOException{
        char[] msg = new char[ len ];
        int readChars = input.read( msg, 0, len );
        if( readChars != 6 ) return null;        // Message is null - end of story
        return new String( msg );
    }

    private String intToString( int value, int length ){    // returns string of length "length", fulfill with zeros
        int intLength = String.valueOf( value ).length();
        if( intLength > length ) return "";
        return "0".repeat( length - intLength ) + String.valueOf( value );
    }

    private String makeHeader( int type, int length ){
            return intToString( type, HEADER_STATEMENT_LENGTH ) + intToString( length, HEADER_LENGTH_LENGTH );
    }

    private int getMsg() throws IOException{
        int[] header = getHeader();
        switch( header[ 0 ] ){
            case STATEMENT -> getStatement( header[ 1 ] );
            case EDIT -> getEdit( header[ 1 ] );
            case PUBLIC_KEY -> getServerPublicKey( header[ 1 ] );
        }
        return 0;
    }

    private int[] getHeader() throws IOException{
        String header = readLine( 6 );
        int[] ret = { -1, -1 };
        if( header != null ){
            int type = Integer.parseInt( header.substring( 4, 8 ) );
            int msgLength = Integer.parseInt( header.substring( 0, 4 ) );
            isAlive = true;
            ret = new int[]{ type, msgLength };
        }
        return ret;
    }

    private void getEdit( int msgLength ) throws IOException{
        Protocol edit = new Protocol( readLine( msgLength ), EDIT );
        System.out.print( edit.getMessage() );
    }

    private void getStatement( int msgLength ) throws IOException{
        Protocol statement = new Protocol( readLine( msgLength ), STATEMENT );
        switch( statement.getStatement() ){
            case KEEP_ALIVE -> isAlive = true;
            case REQUEST_LOGIN -> canLogIn = true;
            case LOGIN_ACCEPTED -> logged = true;
            case LOGIN_REJECTED -> logged = false;
            default -> System.out.print( "No i co z tego???\n" );
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

    public int login () throws IOException{
        //getMsg();
        //if( !canLogIn ) return -1;      TODO na razie wyłączone żebym mógł testować dalej
        Protocol loginMsg = new Protocol( login, password );
        writeMsg( LOGIN, loginMsg.getMessage() );
        System.out.print( logged );
        //if( !logged ) return -1;      TODO na razie wyłączone żebym mógł testować dalej
        return 0;
    }

    private void writeEdit( String msg ){
        Protocol editMsg = new Protocol( msg );
        writeMsg( EDIT, editMsg.getMessage() );
    }

    private void writeStatement( int statement ){
        Protocol statMsg = new Protocol( statement );
        writeMsg( STATEMENT, statMsg.getMessage() );
    }

    private void writeMsg( int type, String msg ){
        String header = makeHeader( type, msg.length() );
        output.println( header );
        output.println( msg );
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
                    if( !( isRunning && getMsg() == 0 ) ) break;
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
