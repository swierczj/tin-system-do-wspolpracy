package client;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import javafx.stage.Modality;
import javafx.stage.Stage;

import java.io.IOException;

public class Connect{
    @FXML public void initialize(){
        setOnDrag();
    }

    private void setOnDrag(){
        dragField.setOnMousePressed( mouseEvent -> {
            x = mouseEvent.getSceneX();
            y = mouseEvent.getSceneY();
        } );

        dragField.setOnMouseDragged( mouseEvent -> {
            Stage stage = ( Stage ) dragField.getScene().getWindow();
            stage.setX( mouseEvent.getScreenX() - x );
            stage.setY( mouseEvent.getScreenY() - y );
            stage.setOpacity( 0.9 );
        } );

        dragField.setOnMouseReleased( mouseEvent -> dragField.getScene().getWindow().setOpacity( 1.0 ) );
    }

    @FXML private void quit(){
        dragField.getScene().getWindow().hide();
        Platform.exit();
        System.exit( 0 );
    }

    @FXML protected void setDefault(){
        if( defaultCheckBox.isSelected() ){
            ipField.setText( "localhost" );
            ipField.setDisable( true );
            portField.setText( "54000" );
            portField.setDisable( true );
        } else{
            ipField.setText( "" );
            ipField.setDisable( false );
            portField.setText( "" );
            portField.setDisable( false );
        }
    }

    @FXML protected void setLoginData() throws IOException{
        ip = ipField.getText();
        try{
            port = Integer.parseInt( portField.getText() );
        } catch( NumberFormatException e ){
            port = 0;
        }
        if( createClient() == 0 )     // hide window
            loginButton.getScene().getWindow().hide();
    }

    private int createClient() throws IOException{
        if( !connected ){                 //TODO uncomment if server is working
            client = new Client( ip, port );
            if( client.connect() != 0 ){
                displayError( "Cannot connect to " + ip + ":" + portField.getText() );
                return -1;
            }
            connected = true;
            System.out.print( "Connected\n" );
        }
//        client.setLoginData( loginField.getText(), passwordField.getText() );
//        if( client.login() != 0 ){
//            client.quit();
//            displayError( "Login or password incorrect." );
//            return -2;
//        }
        client.createGUI();
        return 0;
    }

    private void displayError( String errMsg ){
        Stage stage = new Stage();
        stage.setTitle( "Error" );
        Label l = new Label( errMsg );
        l.setAlignment( Pos.CENTER );
        stage.setScene( new Scene( l, 300, 100 ) );
        stage.initModality( Modality.APPLICATION_MODAL );
        stage.showAndWait();
    }

    @FXML private TextField loginField;
    @FXML private TextField ipField;
    @FXML private TextField portField;
    @FXML private PasswordField passwordField;
    @FXML private CheckBox defaultCheckBox;
    @FXML private Button loginButton;
    @FXML private GridPane dragField;
    private String ip;
    private int port;
    private boolean connected = false;
    private double x = 0, y = 0;

    private Client client;
}
