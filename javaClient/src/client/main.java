package client;

import java.io.IOException;

public class main{
    public static void main( String args[] ) throws IOException{

        String ip = "localhost";
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
