package client;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class main extends Application{

    @Override
    public void start( Stage primaryStage ) throws Exception{
        Parent root = FXMLLoader.load( getClass().getResource( "connect.fxml" ) );
        primaryStage.setTitle( "Connect" );
        primaryStage.setScene( new Scene( root, 392, 213) );
        primaryStage.setResizable( false );
        primaryStage.show();
    }

    public static void main( String[] args ) {
        launch( args );
    }
}
