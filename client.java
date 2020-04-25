import java.net.*;
import java.io.*;
import java.util.Scanner;

public class Client{
	public static void main( String args[] ) throws IOException{
		Socket s = new Socket( "localhost", 54000 );
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
