package client;

import client.Protocol.Header;
import client.Protocol.Key;
import com.google.protobuf.ByteString;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;

import static client.Protocol.Header.MsgType.*;
import static client.Protocol.Key.KeyType.PUBLIC;
import static client.Protocol.Statement.Info.PUBLIC_KEY_REQUEST;

public class Client{
    private static final int HEADER_STATEMENT_LENGTH = 2;
    private static final int HEADER_LENGTH_LENGTH = 2;

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

    private String intToString( int value, int length ){    // returns string of length "length", fulfill with zeros
        int intLength = String.valueOf( value ).length();
        String header = "";
        if( intLength > length ) return "";
        for( int i = length; i > intLength; --i )
            header += "0";
        header += "value";
        return header;
    }

    private String makeHeader( int statement, String msg ){
            return intToString( statement, HEADER_STATEMENT_LENGTH ) + intToString( msg.length(), HEADER_LENGTH_LENGTH );
    }

    private int getHeader() throws IOException{
        String header = input.readLine();
        if( header == null ) return -1;        // Message is null - end of story
        int type = Integer.parseInt( header.substring( 4, 8 ) );
        isAlive = true;
        switch( type ){
            case 1:
                getStatement();
                break;
            case 2:
                getEdit();
                break;
            case 3:
                getServerPublicKey();
                break;
        }
        return 0;
    }

    private void getEdit() throws IOException{
        Protocol.Edit edit = Protocol.Edit.parseFrom( ByteString.copyFrom( input.readLine(), "UTF_8" ) );
        System.out.print( edit.getData() );
    } //TODO

    private void getStatement() throws IOException{
        Protocol.Statement statement = Protocol.Statement.parseFrom( ByteString.copyFrom( input.readLine(), "UTF_8" ) );
        switch( statement.getInfo() ){
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
                System.out.print( "Login rejected. Try again\n");
                login();
                break;
            default:
                System.out.print( "No i co z tego???\n");
        }
    } //TODO

    private void login(){
        //TODO
    }

    private void askForPublicKey(){
        Protocol.Statement.Builder statement = Protocol.Statement.newBuilder();
        statement.setInfo( PUBLIC_KEY_REQUEST );
        String statMsg = statement.toString();
        String headMsg = makeHeader( 1, statMsg );
        output.println( headMsg );
        output.println( statMsg );
    }

    private void getServerPublicKey() throws IOException{
        Key publicKey = Key.parseFrom( ByteString.copyFrom( input.readLine(), "UTF_8" ) );
        if( publicKey.getKeyType() == PUBLIC ){
            serverPublicKey = publicKey.getKey();
            System.out.print( "Key received from server\n" );
        }else{
            askForPublicKey();
            System.out.print( "Key not received. I will try again\n" );
        }
    }

    private void writeEdit( String msg ){
        Protocol.Edit.Builder edit = Protocol.Edit.newBuilder();
        edit.setData( msg );
        String editMsg = edit.toString();
        String headMsg = makeHeader( 2, editMsg );
        output.println( headMsg );
        output.println( editMsg );
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
