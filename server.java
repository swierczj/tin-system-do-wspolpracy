import java.net.*;
import java.io.*;
import java.util.Scanner;

public class server{
	public static void main( String args[] ) throws IOException{
		ServerSocket ss = new ServerSocket( 54000 );
		Socket s = ss.accept();
		System.out.println( "Klient podlaczony" );
		Scanner scan = new Scanner( System.in );
		
		DataInputStream din = new DataInputStream( s.getInputStream() );
		DataOutputStream dout = new DataOutputStream( s.getOutputStream() );
		
		Thread writer = new Thread( () -> {
			try{
				String messageOut = "";
				while( !messageOut.equals( "q" ) && !messageOut.equals( "quit" ) ){
					dout.writeUTF( messageOut );
					messageOut = scan.nextLine();
				}
			} catch( IOException ex ){};
		});
		Thread reader = new Thread( () -> {
			try{	
				String messageIn = "";
				while( true ){
					messageIn = din.readUTF();
					System.out.print( "Michal: " + messageIn + "\n" );
					messageIn = "";
				}
			} catch( IOException ex ){};
		});
		
		writer.start();
		reader.start();
	}
}
