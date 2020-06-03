package client;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

public class main extends Application{

    @Override
    public void start( Stage primaryStage ) throws Exception{
        Parent root = FXMLLoader.load( getClass().getResource( "connect20.fxml" ) );
        primaryStage.setTitle( "Connect" );
        primaryStage.setScene( new Scene( root, 600, 400 ) );
        primaryStage.initStyle( StageStyle.TRANSPARENT );
        primaryStage.setResizable( false );
        primaryStage.show();
    }

    public static void main( String[] args ) {
        launch( args );
    }
}
