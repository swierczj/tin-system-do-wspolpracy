package client;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class main extends Application{

    @Override
    public void start( Stage primaryStage) throws Exception{
        Parent root = FXMLLoader.load(getClass().getResource("generate20.fxml"));
        primaryStage.setTitle("Generator");
        primaryStage.setScene(new Scene(root, 640, 480));
        primaryStage.setResizable( false );
        primaryStage.show();

        String ip = "localhost";
        int port = 54000;

        Client client = new Client( ip, port );
        if( client.connect() == 0 ){
            client.createThreads();
            client.startThreads();
        }
    }

    public static void main(String[] args) {
        launch(args);
    }
}
