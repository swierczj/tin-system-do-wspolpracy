package client;

import java.io.IOException;

public class main{
    public static void main( String args[] ) throws IOException{

        Coder coder = new Coder();
        System.out.println(coder.encrypt("message 1","1234567890123456"));

        String ip = "25.139.176.21";
        int port = 54000;

        if( args.length > 0 )
            ip = args[ 0 ];
        if( args.length > 1 )
            port = Integer.parseInt( args[ 1 ] );

        Client client = new Client( ip, port );
        if( client.connect() == 0 ){
            client.createThreads();

            client.startThreads();
        }

    }
}
