import java.net.*;
import java.io.*;
import java.util.Scanner;

public class client{
	public static void main( String args[] ) throws IOException{
		Socket s = new Socket( "25.139.176.21", 54111 );
		DataInputStream din = new DataInputStream( s.getInputStream() );
		DataOutputStream dout = new DataOutputStream( s.getOutputStream() );
		Scanner scan = new Scanner( System.in );
		
		Thread writer = new Thread( () -> {
			try{
				String messageOut = "";
				while( !messageOut.equals( "q" ) && !messageOut.equals( "quit" ) ){
					dout.writeUTF( messageOut );
					messageOut = scan.nextLine();
				}
				return;
			} catch( IOException ex ){};
		});
		Thread reader = new Thread( () -> {
			try{	
				String messageIn = "";
				while( true ){
					messageIn = din.readUTF();
					System.out.print( "Server: " + messageIn + "\n" );
					messageIn = "";
				}
			} catch( IOException ex ){};
		});
		
		writer.start();
		reader.start();
	}
}
