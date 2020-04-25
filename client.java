import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Client{
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
		
		Thread reader = new Thread( () -> {
			try{
				String messageIn = "";
				while( true ){
					messageIn = din.readLine();
					System.out.print( "Server: " + messageIn + "\n" );
					messageIn = "";
				}
			}catch( IOException ex ){};
		});
		
		Thread writer = new Thread( () -> {
			try{
				String messageOut = "";
				while( !messageOut.equals( "q" ) && !messageOut.equals( "quit" ) ){
					dout.println( messageOut );
					messageOut = scan.nextLine();
				}
				s.close();
				reader.interrupt();
			}catch( IOException ex ){};
		});

		writer.start();
		reader.start();
	}
}
