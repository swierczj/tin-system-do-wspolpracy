import java.net.*;
import java.io.*;
import java.util.Scanner;
import java.util.concurrent.Semaphore;

public class Client{
	public static String messageIn = "";
	public static boolean isAlive = true;
	public static Thread reader, writer, keepAlive;
	
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
				while( true ){
					Thread.sleep( 10000 );
					isAlive = false;
					dout.println( "Am I connected?" );
					Thread.sleep( 5000 );
					if( !isAlive ) break;
				}
				s.close();
				reader.interrupt();
				writer.interrupt();
				System.out.print( "Connection broken" );
			}catch( InterruptedException iEx ){}
			catch( IOException ioEx ){};
		});
			
		reader = new Thread( () -> {
			try{
				while( true ){
					messageIn = din.readLine();
					isAlive = true;
					System.out.print( "Server: " + messageIn + "\n" );
					messageIn = "";
				}
			}catch( IOException ex ){};
		});
		
		writer = new Thread( () -> {
			try{
				String messageOut = "connected";
				while( !messageOut.equals( "q" ) && !messageOut.equals( "quit" ) ){
					dout.println( messageOut );
					messageOut = scan.nextLine();
				}
				s.close();
				reader.interrupt();
				keepAlive.interrupt();
			}catch( IOException ex ){};
		});
		
			

		writer.start();
		reader.start();
		keepAlive.start();
	}
}
