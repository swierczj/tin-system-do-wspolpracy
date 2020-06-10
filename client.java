import java.net.*;
import java.io.*;
import java.util.Scanner;
import java.util.concurrent.Semaphore;

public class Client{
	private static String messageIn = "";
	private static volatile boolean isAlive = true;
	private static Thread reader, writer, keepAlive;
	private static volatile boolean running = true;
	
	public static void main( String args[] ) throws IOException{
		Socket s;
		
		String ip = "localhost";
		int port = 54000;
		
		if( args.length > 0 )
			ip = args[ 0 ];
		if( args.length > 1 )
			port = Integer.parseInt( args[ 1 ] );
		
		try{
			s = new Socket( ip, port );
		}catch( ConnectException cEx ){ 
			System.out.print( "Cannot connect to " + ip + " on port " + port );
			return; 
		};
		
		BufferedReader din = new BufferedReader( new InputStreamReader( s.getInputStream() ) );
		PrintWriter dout = new PrintWriter( s.getOutputStream(), true );
		Scanner scan = new Scanner( System.in );
		
		keepAlive = new Thread( () -> {
			try{
				while( isAlive && running ){
					Thread.sleep( 10000 );
					isAlive = false;
					dout.println( "Am I connected?" );
					Thread.sleep( 5000 );
				}
				System.out.print( "Connection broken" );
				running = false;
				scan.close();
			}catch( InterruptedException iEx ){};
		});
			
		reader = new Thread( () -> {
			try{
				while( running ){
					messageIn = din.readLine();
					if( messageIn == null ) break;
					isAlive = true;
					System.out.print( "Server: " + messageIn + "\n" );
					messageIn = "";
				}
				scan.close();
				System.out.print( "Server disconnected you\n" );
				running = false;
			}catch( IOException ex ){System.out.print( "reader closed\n" );};
		});
		
		writer = new Thread( () -> {
			try{
				String messageOut = "connected";
				while( !messageOut.equals( "q" ) && !messageOut.equals( "quit" ) && running ){
					dout.println( messageOut );
					messageOut = scan.nextLine();
				}
				s.close();
				din.close();
				running = false;
			}
			catch( IOException ex ){};
		});
		
		writer.start();
		reader.start();
		keepAlive.start();
	}
}
